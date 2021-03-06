// -*- Mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-

/*
 * librmf
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 * Copyright (C) 2013-2029 Safran Passenger Innovations
 *
 * Author: Aleksander Morgado <aleksander@aleksander.es>
 */

#ifndef _RMF_OPERATIONS_H_
#define _RMF_OPERATIONS_H_

#include <stdint.h>
#include <vector>
#include <string>

#include "rmf-types.h"

/**
 * Modem:
 *
 * Rave Modem Factory namespace
 */
namespace Modem {

    /**
     * GetManufacturer:
     *
     * Get the modem manufacturer string.
     *
     * Returns: a string.
     */
    std::string GetManufacturer (void);

    /**
     * GetModel:
     *
     * Get the modem model string.
     *
     * Returns: a string.
     */
    std::string GetModel (void);

    /**
     * GetSoftwareRevision:
     *
     * Get the modem software revision string.
     *
     * Returns: a string.
     */
    std::string GetSoftwareRevision (void);

    /**
     * GetHardwareRevision:
     *
     * Get the modem hardware revision string.
     *
     * Returns: a string.
     */
    std::string GetHardwareRevision (void);

    /**
     * GetImei:
     *
     * Get the modem IMEI string.
     *
     * Returns: a string.
     */
    std::string GetImei (void);

    /**
     * GetSimSlot:
     *
     * Get the slot index of the currently active SIM.
     *
     * The setup only expects one or two SIM slots, not more, so the
     * reported indices will be either 1 or 2.
     *
     * If the setup only allows one single SIM, this method will always
     * report index 1.
     *
     * Returns: an integer.
     */
    uint8_t GetSimSlot (void);

    /**
     * SetSimSlot:
     *
     * Select which SIM slot index to activate.
     *
     * If the requested slot index is already the currently active one,
     * this method will do nothing.
     */
    void SetSimSlot (uint8_t slot);

    /**
     * GetImsi:
     *
     * Get the IMSI of the currently active SIM.
     *
     * Returns: a string.
     */
    std::string GetImsi (void);

    /**
     * GetIccid:
     *
     * Get the ICCID of the currently active SIM.
     *
     * Returns: a string.
     */
    std::string GetIccid (void);

    /**
     * GetSimInfo:
     * @operatorMcc:  (out) Mobile Country Code of the operator which issued the SIM, or 0 if unknown.
     * @operatorMnc:  (out) Mobile Network Code of the operator which issued the SIM, or 0 if unknown.
     * @plmns: (out) List of PLMNs configured by the operator.
     *
     * Get additional info from the currently active SIM.
     */
    void GetSimInfo (uint16_t &operatorMcc,
                     uint16_t &operatorMnc,
                     std::vector<struct PlmnInfo>&plmns);

    /**
     * IsSimLocked:
     *
     * Gets whether the SIM is PIN-locked.
     */
    bool IsSimLocked (void);

    /**
     * Unlock:
     * @pin: (in) PIN to send.
     *
     * Unlocks the modem, if needed.
     */
    void Unlock (const std::string pin);

    /**
     * EnablePin:
     * @enable: %true to enable PIN request, %false to disable it.
     * @pin: (in) current PIN.
     *
     * Enables or disables PIN request.
     */
    void EnablePin (bool              enable,
                    const std::string pin);

    /**
     * ChangePin:
     * @pin: (in) current PIN.
     * @newPin: (in) new PIN.
     *
     * Changes the PIN.
     */
    void ChangePin (const std::string pin,
                    const std::string newPin);

    /**
     * GetPowerStatus:
     *
     * Get radio power status.
     *
     * Returns: a #PowerStatus value.
     */
    PowerStatus GetPowerStatus (void);

    /**
     * SetPowerStatus:
     * @power_status: (in) radio power status.
     *
     * Set radio power status.
     */
    void SetPowerStatus (PowerStatus powerStatus);

    /**
     * PowerCycle:
     *
     * Request to power cycle the modem.
     */
    void PowerCycle (void);

    /**
     * GetPowerInfo:
     *
     * Get the radio power information.
     *
     * This information is given separately for each available radio access
     * technology (e.g. GSM, UMTS or LTE).
     *
     * Returns: a vector of #RadioPowerInfo structs.
     */
    std::vector<RadioPowerInfo> GetPowerInfo (void);

    /**
     * GetSignalInfo:
     * @signalInfo: (out)
     *
     * Get the signal quality information.
     *
     * This information is given separately for each available radio access
     * technology (e.g. GSM, UMTS or LTE).
     *
     * Returns: a vector of #RadioSignalInfo structs.
     */
    std::vector<RadioSignalInfo> GetSignalInfo (void);

    /**
     * GetRegistrationStatus:
     * @operatorDescription: (out) description string of the operator, or empty
     *                        string if unknown.
     * @operatorMcc: (out) Mobile Country Code of the operator, or 0 if unknown.
     * @operatorMnc: (out) Mobile Network Code of the operator, or 0 if unknown.
     * @lac: (out) Location Area Code, or 0 if unknown.
     * @cid: (out) Cell ID, or 0 if unknown.
     *
     * Get the network registration (serving system) info.
     *
     * Returns: the status of the registration.
     */
    RegistrationStatus GetRegistrationStatus (std::string   &operatorDescription,
                                              uint16_t      &operatorMcc,
                                              uint16_t      &operatorMnc,
                                              uint16_t      &lac,
                                              uint32_t      &cid);

    /**
     * GetRegistrationTimeout:
     *
     * Gets the internal registration timeout.
     *
     * Returns: number of seconds to consider a registration attempt as timed out.
     */
    uint32_t GetRegistrationTimeout (void);


    /**
     * SetRegistrationTimeout:
     * @timeout: number of seconds to consider a registration attempt as timed out.
     *
     * Sets the internal registration timeout.
     */
    void SetRegistrationTimeout (uint32_t timeout);

    /**
     * GetConnectionStatus:
     *
     * Get connection status.
     *
     * Returns: the status of the connection.
     */
    ConnectionStatus GetConnectionStatus (void);

    /**
     * GetConnectionStats:
     * @txPacketsOk: (out) amount of packets transmitted without error.
     * @rxPacketsOk: (out) amount of packets received without error.
     * @txPacketsError: (out) amount of outgoing packets with framing errors.
     * @rxPacketsError: (out) amount of incoming packets with framing errors.
     * @txPacketsOverflow: (out) amount of packes dropped because transmitter
     *                       buffer overflowed.
     * @rxPacketsOverflow: (out) amount of packes dropped because receiver
     *                       buffer overflowed.
     * @txBytesOk: (out) amount of bytes transmitted without error.
     * @rxBytesOk: (out) amount of bytes received without error.
     *
     * Get connection stats.
     *
     * Returns: %true if the connection stats are valid, %false otherwise.
     */
    bool GetConnectionStats (uint32_t &txPacketsOk,
                             uint32_t &rxPacketsOk,
                             uint32_t &txPacketsError,
                             uint32_t &rxPacketsError,
                             uint32_t &txPacketsOverflow,
                             uint32_t &rxPacketsOverflow,
                             uint64_t &txBytesOk,
                             uint64_t &rxBytesOk);

    /**
     * Connect:
     * @apn: (in) Access Point Name.
     * @user: (in) username to use when authenticating in the access point, if needed.
     * @password: (in) password to use when authenticating in the access point, if needed.
     *
     * Request connection to the network using IPv4.
     */
    void Connect (const std::string apn,
                  const std::string user,
                  const std::string password);

    /**
     * Disconnect:
     *
     * Request disconnection from the network.
     */
    void Disconnect (void);

    /**
     * GetDataPort:
     *
     * Get the name of the data port.
     *
     * Returns: a string.
     */
    std::string GetDataPort (void);

    /**
     * IsModemAvailable:
     *
     * Gets whether a modem is available.
     */
    bool IsModemAvailable (void);

    /**
     * SetTargetRemote:
     *
     * Specifies the IP address and TCP port where the target rmfd daemon is
     * listening. All operations executed after this call will be performed with
     * the rmfd daemon listening in the remote address.
     */
    bool SetTargetRemote (const std::string address,
                          uint16_t          port);

    /**
     * SetTargetLocal:
     *
     * All operations executed after this call will be performed with the rmfd
     * daemon listening in the local unix socket.
     *
     * This is the default mode of operation if SetTargetRemote() has never been
     * called before.
     */
    bool SetTargetLocal (void);
}

#endif /* _RMF_OPERATIONS_H_ */
