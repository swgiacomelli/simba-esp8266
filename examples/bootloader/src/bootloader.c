/**
 * @file bootloader.c
 * @version 0.2.0
 *
 * @section License
 * Copyright (C) 2015-2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Simba project.
 */

#include "simba.h"

#include "bootloader.h"
#include "arpa/inet.h"

#define APPLICATION_VALID_FLAG                             0xbe

/* The maximum data transfer size. */
#define TRANSFER_DATA_SIZE_MAX                             4096

/* Data IDentifiers (DID). */
#define DID_BOOTLOADER_VERSION                           0xf000
#define DID_SYSTEM_TIME                                  0xf001

/* Diagnostic session types. */
#define DIAGNOSTIC_SESSION_CONTROL_DEFAULT                 0x02

/* UDS routine identifiers. */
#define DIAGNOSTIC_SESSION_CONTROL                         0x10
#define READ_DATA_BY_IDENTIFIER                            0x22
#define REQUEST_DOWNLOAD                                   0x34
#define TRANSFER_DATA                                      0x36
#define REQUEST_TRANSFER_EXIT                              0x37

/* Positive response bit. */
#define POSITIVE_RESPONSE                                  0x40

/*Negative response codes. */
#define GENERAL_REJECT                                     0x10
#define SERVICE_NOT_SUPPORTED                              0x11
#define SUBFUNCTION_NOT_SUPPORTED                          0x12
#define INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT         0x13
#define RESPONSE_TOO_LONG                                  0x14
#define BUSY_REPEAT_REQUEST                                0x21
#define CONDITIONS_NOT_CORRECT                             0x22
#define REQUEST_SEQUENCE_ERROR                             0x24
#define NO_RESPONSE_FROM_SUBNET_COMPONENT                  0x25
#define FAILURE_PREVENTS_EXECUTION_OF_REQUESTED_ACTION     0x26
#define REQUEST_OUT_OF_RANGE                               0x31
#define SECURITY_ACCESS_DENIED                             0x33
#define INVALID_KEY                                        0x35
#define EXCEED_NUMBER_OF_ATTEMPTS                          0x36
#define REQUIRED_TIME_DELAY_NOT_EXPIRED                    0x37
#define UPLOAD_DOWNLOAD_NOT_ACCEPTED                       0x70
#define TRANSFER_DATA_SUSPENDED                            0x71
#define GENERAL_PROGRAMMING_FAILURE                        0x72
#define WRONG_BLOCK_SEQUENCE_NUMBER                        0x73
#define REQUEST_CORRECTLY_RECEIVED_BUT_RESPONSE_IS_PENDING 0x78
#define SUBFUNCTION_NOT_SUPPORTED_INACTIVE_SESSION         0x7e
#define SERVICE_NOT_SUPPORTED_INACTIVE_SESSION             0x7f
#define RPM_TOO_HIGH                                       0x81
#define RPM_TO_LOW                                         0x82
#define ENGINE_IS_RUNNING                                  0x83
#define ENGINE_IS_NOT_RUNNING                              0x84
#define ENGINE_RUNTIME_TOO_LOW                             0x85
#define TEMPERATURE_TOO_HIGH                               0x86
#define TEMPERATURE_TOO_LOW                                0x87
#define VEHICLE_SPEED_TOO_HIGH                             0x88
#define VEHICLE_SPEED_TOO_LOW                              0x89
#define THROTTLE_PEDAL_TOO_HIGH                            0x8a
#define THROTTLE_PEDAL_TOO_LOW                             0x8b
#define TRANSMISSION_RANGE_NOT_IN_NEUTRAL                  0x8c
#define TRANSMISSION_RANGE_NOT_IN_GEAR                     0x8d
#define BRAKE_SWITCH_NOT_CLOSED                            0x8f
#define SHIFTER_LEVER_NOT_IN_PARK                          0x90
#define TORQUE_CONVERTER_CLUTCH_LOCKED                     0x91
#define VOLTAGE_TOO_HIGH                                   0x92
#define VOLTAGE_TOO_LOW                                    0x93

static int ignore(struct bootloader_t *self_p,
                  int size)
{
    uint8_t dummy;

    while (size > 0) {
        chan_read(self_p->chin_p, &dummy, sizeof(dummy));
        size--;
    }

    return (0);
}

static int erase_application(struct bootloader_t *self_p)
{
    return (0);
}

static int write_application(struct bootloader_t *self_p,
                             size_t size)
{
    uint8_t dummy;

    /* Write the data to the memory. */
    while (size > 0) {
        chan_read(self_p->chin_p, &dummy, 1);
        size--;
    }

    return (0);
}

static int is_application_valid(struct bootloader_t *self_p)
{
    uint32_t flag_address;

    flag_address = (self_p->application_address + self_p->application_size);

    return (*(uint8_t *)(uintptr_t)flag_address == APPLICATION_VALID_FLAG);
}

static int write_application_valid_flag(struct bootloader_t *self_p)
{
    uint8_t flag;
    uint32_t flag_address;

    /* Write the flag at the end of the application flash area. */
    flag = APPLICATION_VALID_FLAG;
    flag_address = (self_p->application_address + self_p->application_size);

    (void)flag;
    (void)flag_address;
    //flash_write(flag_address, &flag, sizeof(flag));

    return (0);
}

static int write_response(struct bootloader_t *self_p,
                          int32_t length,
                          uint8_t code)
{
    length = htonl(length + 1);
    chan_write(self_p->chout_p, &length, sizeof(length));
    chan_write(self_p->chout_p, &code, sizeof(code));

    return (0);
}

static int ignore_and_write_response_no_data(struct bootloader_t *self_p,
                                             size_t length,
                                             uint8_t code)
{
    ignore(self_p, length);

    return (write_response(self_p, 0, code));
}

static int write_did_response(chan_t *chout_p,
                              uint8_t response_code,
                              uint16_t did,
                              const void *data_p,
                              size_t size)
{
    uint32_t length;

    length = (sizeof(response_code)
              + sizeof(did)
              + size);

    length = htonl(length);
    did = htons(did);

    chan_write(chout_p, &length, sizeof(length));
    chan_write(chout_p, &response_code, sizeof(response_code));
    chan_write(chout_p, &did, sizeof(did));
    chan_write(chout_p, data_p, size);

    return (0);
}

static int handle_read_data_by_identifier_version(struct bootloader_t *self_p)
{
    const char version[] = STRINGIFY(VERSION);

    return (write_did_response(self_p->chout_p,
                               (READ_DATA_BY_IDENTIFIER | POSITIVE_RESPONSE),
                               DID_BOOTLOADER_VERSION,
                               version,
                               sizeof(version)));
}

static int handle_read_data_by_identifier_system_time(struct bootloader_t *self_p)
{
    char buf[32];
    struct time_t now;

    time_get(&now);
    std_sprintf(buf, FSTR("%lu"), now.seconds);

    return (write_did_response(self_p->chout_p,
                               (READ_DATA_BY_IDENTIFIER | POSITIVE_RESPONSE),
                               DID_SYSTEM_TIME,
                               buf,
                               strlen(buf) + 1));
}

/**
 * Enter the default session.
 */
static int handle_diagnostic_session_control_default(struct bootloader_t *self_p)
{
    /* if (is_application_valid(self_p) == 0) { */
    /*     ignore_and_write_response_no_data(self_p, */
    /*                                       0, */
    /*                                       CONDITIONS_NOT_CORRECT); */

    /*     return (-1); */
    /* } */

    ignore_and_write_response_no_data(self_p,
                                      0,
                                      (DIAGNOSTIC_SESSION_CONTROL | POSITIVE_RESPONSE));

    /* Call the application. */
    /* ((void (*)(void))(uintptr_t)self_p->application_address)(); */

    return (0);
}

/**
 * UDS uses different operating sessions, which can be changed using
 * the "Diagnostic Session Control". Depending on which session is
 * active, different services are available. On start, the control
 * unit is by default in the "Default Session". Other sessions are
 * defined, but are not required to be implemented depending on the
 * type of device:
 *
 * - "Programming Session" used to upload software.
 *
 * - "Extended Diagnostic Session" used to unlock additional
 *   diagnostic functions, such as the adjustment of sensors.
 *
 * - "Safety system diagnostic session" used to test all
 *   safety-critical diagnostic functions, such as airbag tests.
 *
 * In addition, there are reserved session identifiers that can be
 * defined for vehicle manufacturers and vehicle suppliers specific
 * use.
 */
static int handle_diagnostic_session_control(struct bootloader_t *self_p,
                                             int length)
{
    uint8_t session;
    int res;

    /* Length check. */
    if (length < 1) {
        ignore_and_write_response_no_data(self_p,
                                          0,
                                          INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

        return (-1);
    }

    /* Read session type. */
    chan_read(self_p->chin_p, &session, sizeof(session));
    length -= 1;

    /* Handle the session. */
    switch (session) {

    case DIAGNOSTIC_SESSION_CONTROL_DEFAULT:
        res = handle_diagnostic_session_control_default(self_p);
        break;

    default:
        ignore_and_write_response_no_data(self_p, 0, REQUEST_OUT_OF_RANGE);
        res = -1;
        break;
    }

    return (res);
}

/**
 * With this service it is possible to retrieve one or more values of
 * a control unit. This can be information of all kinds and of
 * different lengths such as Partnumber or the software
 * version. Dynamic values such as the current state of the sensor
 * can be queried. Each value is associated to a Data Identifier
 * (DID) between 0 and 65535.
 */
static int handle_read_data_by_identifier(struct bootloader_t *self_p,
                                          int length)
{
    uint16_t did;
    int res;

    if (length < 2) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

        return (-1);
    }

    chan_read(self_p->chin_p, &did, sizeof(did));

    did = ntohs(did);
    length -= 2;

    switch (did) {

    case DID_BOOTLOADER_VERSION:
        res = handle_read_data_by_identifier_version(self_p);
        break;

    case DID_SYSTEM_TIME:
        res = handle_read_data_by_identifier_system_time(self_p);
        break;

    default:
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          SERVICE_NOT_SUPPORTED);
        res = -1;
        break;
    }

    return (res);
}

/**
 * Downloading new software or other data into the control unit is
 * introduced using the "Request Download". Here, the location and
 * size of the data is specified. In turn, the controller specifies
 * how large the data packets can be.
 */
static int handle_request_download(struct bootloader_t *self_p,
                                   int length)
{
    uint8_t buf[5];
    uint32_t address;
    uint32_t size;

    /* Length check. */
    if (length < 3) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

        return (-1);
    }

    chan_read(self_p->chin_p, &buf, 3);
    length -= 3;

    /* The address and size should be 4 bytes each. */
    if ((buf[1] != sizeof(address)) || (buf[2] != sizeof(size))) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          REQUEST_OUT_OF_RANGE);

        return (-1);
    }

    /* Length check. */
    if (length < 8) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

        return (-1);
    }

    chan_read(self_p->chin_p, &address, sizeof(address));
    chan_read(self_p->chin_p, &size, sizeof(size));
    length -= 8;

    /* Save the address and size. */
    self_p->swdl.address = ntohl(address);
    self_p->swdl.size = ntohl(size);
    self_p->swdl.offset = 0;
    self_p->swdl.next_block_sequence_counter = 0;

    if (self_p->swdl.address == self_p->application_address) {
        if (self_p->swdl.size > (self_p->application_size)) {
            ignore_and_write_response_no_data(self_p,
                                              length,
                                              REQUEST_OUT_OF_RANGE);

            return (-1);
        }

        erase_application(self_p);
    } else {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          REQUEST_OUT_OF_RANGE);

        return (-1);
    }

    self_p->state = STATE_SWDL;

    /* Send the positive response. */
    write_response(self_p,
                   sizeof(buf),
                   (REQUEST_DOWNLOAD | POSITIVE_RESPONSE));

    buf[0] = 0x40;
    buf[1] = ((TRANSFER_DATA_SIZE_MAX >> 24) & 0xff);
    buf[2] = ((TRANSFER_DATA_SIZE_MAX >> 16) & 0xff);
    buf[3] = ((TRANSFER_DATA_SIZE_MAX >> 8) & 0xff);
    buf[4] = ((TRANSFER_DATA_SIZE_MAX >> 0) & 0xff);

    chan_write(self_p->chout_p, buf, sizeof(buf));

    return (0);
}

/**
 * For the actual transmission of data, the service "Transfer Data" is
 * used. This service is used for both uploading and downloading
 * data. The transfer direction is notified in advance by the service
 * "Request Download" or "Upload Request". This service should try to
 * send packets at maximum length, as specified in previous
 * services. If the data set is larger than the maximum, the "Transfer
 * Data" service must be used several times in succession until all
 * data has arrived.
 */
static int handle_transfer_data(struct bootloader_t *self_p,
                                int length)
{
    int res;
    uint8_t block_sequence_counter;

    /* A request download request must be sent before data can be
       transferred. */
    if (self_p->state != STATE_SWDL) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          CONDITIONS_NOT_CORRECT);

        return (-1);
    }

    /* Length check. */
    if (length < 1) {
        ignore_and_write_response_no_data(self_p,
                                          0,
                                          INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

        return (-1);
    }

    /* Sequence counter. */
    chan_read(self_p->chin_p, &block_sequence_counter, sizeof(block_sequence_counter));
    length--;

    /* Only accept the expected sequence counter. */
    if (block_sequence_counter != self_p->swdl.next_block_sequence_counter) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          WRONG_BLOCK_SEQUENCE_NUMBER);

        return (-1);
    }

    /* Don't write outside the allowed memory region. */
    if (length > (self_p->swdl.size - self_p->swdl.offset)) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          REQUEST_OUT_OF_RANGE);

        return (-1);
    }

    /* Write to the memory. */
    if (write_application(self_p, length) == 0) {
        self_p->swdl.offset += length;
        self_p->swdl.next_block_sequence_counter++;
        write_response(self_p,
                       sizeof(block_sequence_counter),
                       (TRANSFER_DATA | POSITIVE_RESPONSE));
        chan_write(self_p->chout_p,
                   &block_sequence_counter,
                   sizeof(block_sequence_counter));
        res = 0;
    } else {
        ignore_and_write_response_no_data(self_p,
                                          0,
                                          GENERAL_PROGRAMMING_FAILURE);
        res = -1;
    }

    return (res);
}

/**
 * A data transmission can be 'completed' when using the "Transfer
 * Exit" service. This service is used for comparison between the
 * control unit and the tester. When it is running, a control unit
 * can answer negatively on this request to stop a data transfer
 * request. This will be used when the amount of data (set in
 * "Request Download" or "Upload Request") has not been transferred.
 */
static int handle_request_transfer_exit(struct bootloader_t *self_p,
                                        int length)
{
    /* Sanity check. */
    if (length != 0) {
        ignore_and_write_response_no_data(self_p,
                                          length,
                                          INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

        return (-1);
    }

    /* Can only exit if swdl is active. */
    if (self_p->state != STATE_SWDL) {
        ignore_and_write_response_no_data(self_p,
                                          0,
                                          CONDITIONS_NOT_CORRECT);

        return (-1);
    }

    /* Write the application valid flag if the whole application has
       been written. */
    if (self_p->swdl.offset == self_p->swdl.size) {
        write_application_valid_flag(self_p);
    }

    self_p->state = STATE_IDLE;

    ignore_and_write_response_no_data(self_p,
                                      0,
                                      (REQUEST_TRANSFER_EXIT | POSITIVE_RESPONSE));

    return (0);
}

/**
 * Send a negative response to unknown service id:s.
 */
static int handle_unknown_service_id(struct bootloader_t *self_p,
                                     int length)
{
    ignore_and_write_response_no_data(self_p,
                                      length,
                                      SERVICE_NOT_SUPPORTED);

    return (0);
}

int bootloader_init(struct bootloader_t *self_p,
                    chan_t *chin_p,
                    chan_t *chout_p,
                    uint32_t application_address,
                    uint32_t application_size)
{
    self_p->state = STATE_IDLE;
    self_p->chin_p = chin_p;
    self_p->chout_p = chout_p;
    self_p->application_address = application_address;
    /* Subtract one for the application valid flag. */
    self_p->application_size = (application_size - 1);

    return (0);
}

int bootloader_handle_service(struct bootloader_t *self_p)
{
    int32_t length;
    int8_t service_id;
    int res;

    /* Wait for a service header. */
    chan_read(self_p->chin_p, &length, sizeof(length));
    length = ntohl(length);

    /* The minimum packet size is one byte. */
    if (length < 1) {
        return (-1);
    }

    /* Read the service id. */
    chan_read(self_p->chin_p, &service_id, sizeof(service_id));
    length -= 1;

    /* Handle the service. */
    switch (service_id) {

    case DIAGNOSTIC_SESSION_CONTROL:
        res = handle_diagnostic_session_control(self_p, length);
        break;

    case READ_DATA_BY_IDENTIFIER:
        res = handle_read_data_by_identifier(self_p, length);
        break;

    case REQUEST_DOWNLOAD:
        res = handle_request_download(self_p, length);
        break;

    case TRANSFER_DATA:
        res = handle_transfer_data(self_p, length);
        break;

    case REQUEST_TRANSFER_EXIT:
        res = handle_request_transfer_exit(self_p, length);
        break;

    default:
        res = handle_unknown_service_id(self_p, length);
        break;
    }

    return (res);
}

void bootloader_main(struct bootloader_t *self_p)
{
    struct pin_driver_t stay_in_bootloader_pin;

    pin_init(&stay_in_bootloader_pin, &pin_d2_dev, PIN_INPUT);

    /* Check the "stay in bootloader" pin. */
    if (pin_read(&stay_in_bootloader_pin) == 0) {
        /* Call the application if it is valid. */
        if (is_application_valid(self_p) == 1) {
            ((void (*)(void))(uintptr_t)self_p->application_address)();
        }
    }

    /* Enter the bootloader main loop. */
    while (1) {
        bootloader_handle_service(self_p);
    }
}
