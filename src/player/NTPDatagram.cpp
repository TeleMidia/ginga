/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "player/NTPDatagram.h"

NTPDatagram::NTPDatagram() {
	memset(stream, 0x00, NTP_BUFFER_SIZE);
	leap_indicator = 0;
	version_number = 0;
	mode = 0;
	stratum = 0;
	poll_interval = 0;
	precision = 0;
	root_delay = 0;
	root_dispersion = 0;
	ref_id = 0;
	ref_timestamp1 = 0;
	ref_timestamp2 = 0;
	orig_timestamp1 = 0;
	orig_timestamp2 = 0;
	recv_timestamp1 = 0;
	recv_timestamp2 = 0;
	trans_timestamp1 = 0;
	trans_timestamp2 = 0;
	key_id = 0;
	msg_digest1 = 0;
	msg_digest2 = 0;
	msg_digest3 = 0;
	msg_digest4 = 0;
}

NTPDatagram::NTPDatagram(char* ntpStream) {

	memcpy(this->stream, ntpStream, NTP_BUFFER_SIZE);
	//Convert the NTP data received to host byte order
	NTPDatagram::convertNTPDataToHost((alt_u8*)stream);

	//NTP Control Header Decode
	precision = stream[0];
	poll_interval = stream[1];
	stratum = stream[2];
	mode = stream[3] & NTP_CONT_HEAD_MODE_MSK;
	version_number = (stream[3] >> 3) & NTP_CONT_HEAD_VN_MSK;
	leap_indicator = (stream[3] >> 6) & NTP_CONT_HEAD_LI_MSK;

	//NTP Root Delay Decode
	 root_delay       = *NTP_DATA_GRAM_ROOT_DELAY(stream);
	//NTP Root Dispersion Decode
	 root_dispersion  = *NTP_DATA_GRAM_ROOT_DISP(stream);
	//NTP Reference ID Decode
	 ref_id           = *NTP_DATA_GRAM_REF_ID(stream);
	//NTP Reference Timestamp Decode
	 ref_timestamp1   = *NTP_DATA_GRAM_REF_TSTMP_INT(stream);
	 ref_timestamp2   = *NTP_DATA_GRAM_REF_TSTMP_FRAC(stream);
	//NTP Originate Timestamp Decode
	 orig_timestamp1  = *NTP_DATA_GRAM_ORIG_TSTMP_INT(stream);
	 orig_timestamp2  = *NTP_DATA_GRAM_ORIG_TSTMP_FRAC(stream);
	//NTP Receive Timestamp Decode
	 recv_timestamp1  = *NTP_DATA_GRAM_RECV_TSTMP_INT(stream);
	 recv_timestamp2  = *NTP_DATA_GRAM_RECV_TSTMP_FRAC(stream);
	//NTP Transmit Timestamp Decode
	 trans_timestamp1 = *NTP_DATA_GRAM_TRANS_TSTMP_INT(stream);
	 trans_timestamp2 = *NTP_DATA_GRAM_TRANS_TSTMP_FRAC(stream);
	//NTP Key ID Decode
	 key_id           = *NTP_DATA_GRAM_KEY_ID(stream);
	//NTP Message Digest Decode
	 msg_digest1      = *NTP_DATA_GRAM_MSG_DIG1(stream);
	 msg_digest2      = *NTP_DATA_GRAM_MSG_DIG2(stream);
	 msg_digest3      = *NTP_DATA_GRAM_MSG_DIG3(stream);
	 msg_digest4      = *NTP_DATA_GRAM_MSG_DIG4(stream);
}

NTPDatagram::~NTPDatagram() {

}

/*
 * The purpose of this function is to encode NTP data from a
 * ntp_data_gram_struc and place it in a NTP buffer in a order ready for
 * transmission.
 */

int NTPDatagram::updateStream() {
	memset(stream, 0x00, NTP_BUFFER_SIZE);

	//NTP Control Header Encode
	stream[0] = precision & 0xFF;
	stream[1] = poll_interval & 0xFF;
	stream[2] = stratum & 0xFF;
	stream[3] = mode & NTP_CONT_HEAD_MODE_MSK;
	stream[3] = stream[3] | ((version_number & NTP_CONT_HEAD_VN_MSK) << 3);
	stream[3] = stream[3] | ((leap_indicator & NTP_CONT_HEAD_LI_MSK) << 6);

	//NTP Root Delay Encode
	*NTP_DATA_GRAM_ROOT_DELAY(stream)       = root_delay;
	//NTP Root Dispersion Encode
	*NTP_DATA_GRAM_ROOT_DISP(stream)        = root_dispersion;
	//NTP Reference ID Encode
	*NTP_DATA_GRAM_REF_ID(stream)           = ref_id;
	//NTP Reference Timestamp Encode
	*NTP_DATA_GRAM_REF_TSTMP_INT(stream)    = ref_timestamp1;
	*NTP_DATA_GRAM_REF_TSTMP_FRAC(stream)   = ref_timestamp2;
	//NTP Originate Timestamp Encode
	*NTP_DATA_GRAM_ORIG_TSTMP_INT(stream)   = orig_timestamp1;
	*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(stream)  = orig_timestamp2;
	//NTP Receive Timestamp Encode
	*NTP_DATA_GRAM_RECV_TSTMP_INT(stream)   = recv_timestamp1;
	*NTP_DATA_GRAM_RECV_TSTMP_FRAC(stream)  = recv_timestamp2;
	//NTP Transmit Timestamp Encode
	*NTP_DATA_GRAM_TRANS_TSTMP_INT(stream)  = trans_timestamp1;
	*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(stream) = trans_timestamp2;
	//NTP Key ID Encode
	*NTP_DATA_GRAM_KEY_ID(stream)           = key_id;
	//NTP Message Digest Encode
	*NTP_DATA_GRAM_MSG_DIG1(stream)         = msg_digest1;
	*NTP_DATA_GRAM_MSG_DIG2(stream)         = msg_digest2;
	*NTP_DATA_GRAM_MSG_DIG3(stream)         = msg_digest3;
	*NTP_DATA_GRAM_MSG_DIG4(stream)         = msg_digest4;

	//Convert the NTP host data to network byte order
	NTPDatagram::convertNTPDataToNet(stream);

	return NTP_BUFFER_SIZE;
}

/*
 * The purpose of this function is to convert an NTP buffer area from network
 * byte order to host byte order.
 */
int NTPDatagram::convertNTPDataToHost(alt_u8* buffer) {
	*NTP_DATA_GRAM_CONTROL_HEADER(buffer) =
	ntohl(*NTP_DATA_GRAM_CONTROL_HEADER(buffer));

	*NTP_DATA_GRAM_ROOT_DISP(buffer) =
	ntohl(*NTP_DATA_GRAM_ROOT_DISP(buffer));

	*NTP_DATA_GRAM_REF_ID(buffer) =
	ntohl(*NTP_DATA_GRAM_REF_ID(buffer));

	*NTP_DATA_GRAM_REF_TSTMP_INT(buffer) =
	ntohl(*NTP_DATA_GRAM_REF_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_REF_TSTMP_FRAC(buffer) =
	ntohl(*NTP_DATA_GRAM_REF_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_ORIG_TSTMP_INT(buffer) =
	ntohl(*NTP_DATA_GRAM_ORIG_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(buffer) =
	ntohl(*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_RECV_TSTMP_INT(buffer) =
	ntohl(*NTP_DATA_GRAM_RECV_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_RECV_TSTMP_FRAC(buffer) =
	ntohl(*NTP_DATA_GRAM_RECV_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_TRANS_TSTMP_INT(buffer) =
	ntohl(*NTP_DATA_GRAM_TRANS_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(buffer) =
	ntohl(*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_KEY_ID(buffer) =
	ntohl(*NTP_DATA_GRAM_KEY_ID(buffer));

	*NTP_DATA_GRAM_MSG_DIG1(buffer) =
	ntohl(*NTP_DATA_GRAM_MSG_DIG1(buffer));

	*NTP_DATA_GRAM_MSG_DIG2(buffer) =
	ntohl(*NTP_DATA_GRAM_MSG_DIG2(buffer));

	*NTP_DATA_GRAM_MSG_DIG3(buffer) =
	ntohl(*NTP_DATA_GRAM_MSG_DIG3(buffer));

	*NTP_DATA_GRAM_MSG_DIG4(buffer) =
	ntohl(*NTP_DATA_GRAM_MSG_DIG4(buffer));

	return 0;
}

/*
 * The purpose of this function is to convert an NTP buffer area from host
 * byte order to network byte order.
 */
int NTPDatagram::convertNTPDataToNet(alt_u8 *buffer) {
	*NTP_DATA_GRAM_CONTROL_HEADER(buffer) =
	htonl(*NTP_DATA_GRAM_CONTROL_HEADER(buffer));

	*NTP_DATA_GRAM_ROOT_DISP(buffer) =
	htonl(*NTP_DATA_GRAM_ROOT_DISP(buffer));

	*NTP_DATA_GRAM_REF_ID(buffer) =
	htonl(*NTP_DATA_GRAM_REF_ID(buffer));

	*NTP_DATA_GRAM_REF_TSTMP_INT(buffer) =
	htonl(*NTP_DATA_GRAM_REF_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_REF_TSTMP_FRAC(buffer) =
	htonl(*NTP_DATA_GRAM_REF_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_ORIG_TSTMP_INT(buffer) =
	htonl(*NTP_DATA_GRAM_ORIG_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(buffer) =
	htonl(*NTP_DATA_GRAM_ORIG_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_RECV_TSTMP_INT(buffer) =
	htonl(*NTP_DATA_GRAM_RECV_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_RECV_TSTMP_FRAC(buffer) =
	htonl(*NTP_DATA_GRAM_RECV_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_TRANS_TSTMP_INT(buffer) =
	htonl(*NTP_DATA_GRAM_TRANS_TSTMP_INT(buffer));

	*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(buffer) =
	htonl(*NTP_DATA_GRAM_TRANS_TSTMP_FRAC(buffer));

	*NTP_DATA_GRAM_KEY_ID(buffer) =
	htonl(*NTP_DATA_GRAM_KEY_ID(buffer));

	*NTP_DATA_GRAM_MSG_DIG1(buffer) =
	htonl(*NTP_DATA_GRAM_MSG_DIG1(buffer));

	*NTP_DATA_GRAM_MSG_DIG2(buffer) =
	htonl(*NTP_DATA_GRAM_MSG_DIG2(buffer));

	*NTP_DATA_GRAM_MSG_DIG3(buffer) =
	htonl(*NTP_DATA_GRAM_MSG_DIG3(buffer));

	*NTP_DATA_GRAM_MSG_DIG4(buffer) =
	htonl(*NTP_DATA_GRAM_MSG_DIG4(buffer));

	return 0;
}

void NTPDatagram::setLeapIndicator(alt_u8 li) {
	leap_indicator = li;
}

void NTPDatagram::setVersionNumber(alt_u8 vn) {
	version_number = vn;
}

void NTPDatagram::setMode(alt_u8 m) {
	mode = m;
}

void NTPDatagram::setStratum(alt_u8 s) {
	stratum = s;
}

void NTPDatagram::setPoll_interval(alt_8 pi) {
	poll_interval = pi;
}

void NTPDatagram::setPrecision(alt_8 p) {
	precision = p;
}

void NTPDatagram::setRootDelay(alt_32 rd) {
	root_delay = rd;
}

void NTPDatagram::setRootDispersion(alt_u32 rd) {
	root_dispersion = rd;
}
;
void NTPDatagram::setRefId(alt_u32 id) {
	ref_id = id;
}

void NTPDatagram::setRefTimestamp1(alt_u32 rt1) {
	ref_timestamp1 = rt1;
}

void NTPDatagram::setRefTimestamp2(alt_u32 rt2) {
	ref_timestamp2 = rt2;
}

void NTPDatagram::setOrigTimestamp1(alt_u32 ot1) {
	orig_timestamp1 = ot1;
}

void NTPDatagram::setOrigTimestamp2(alt_u32 ot2) {
	orig_timestamp2 = ot2;
}

void NTPDatagram::setRecvTimestamp1(alt_u32 rt1) {
	recv_timestamp1 = rt1;
}

void NTPDatagram::setRecvTimestamp2(alt_u32 rt2) {
	recv_timestamp2 = rt2;
}

void NTPDatagram::setTransTimestamp1(alt_u32 tt1) {
	trans_timestamp1 = tt1;
}

void NTPDatagram::setTransTimestamp2(alt_u32 tt2) {
	trans_timestamp2 = tt2;
}

void NTPDatagram::setKeyId(alt_u32 id) {
	key_id = id;
}

void NTPDatagram::setMsgDigest1(alt_u32 md1) {
	msg_digest1 = md1;
}

void NTPDatagram::setMsgDigest2(alt_u32 md2) {
	msg_digest2 = md2;
}

void NTPDatagram::setMsgDigest3(alt_u32 md3) {
	msg_digest3 = md3;
}

void NTPDatagram::setMsgDigest4(alt_u32 md4) {
	msg_digest4 = md4;
}

alt_u8 NTPDatagram::getLeapIndicator() {
	return leap_indicator;
}

alt_u8 NTPDatagram::getVersionNumber() {
	return version_number;
}

alt_u8 NTPDatagram::getMode() {
	return mode;
}

alt_u8 NTPDatagram::getStratum() {
	return stratum;
}

alt_8 NTPDatagram::getPollInterval() {
	return poll_interval;
}

alt_8 NTPDatagram::getPrecision() {
	return precision;
}

alt_32 NTPDatagram::getRootDelay() {
	return root_delay;
}

alt_u32 NTPDatagram::getRootDispersion() {
	return root_dispersion;
}

alt_u32 NTPDatagram::getRefId() {
	return ref_id;
}

alt_u32 NTPDatagram::getRefTimestamp1() {
	return ref_timestamp1;
}

alt_u32 NTPDatagram::getRefTimestamp2() {
	return ref_timestamp2;
}

alt_u32 NTPDatagram::getOrigTimestamp1() {
	return orig_timestamp1;
}

alt_u32 NTPDatagram::getOrigTimestamp2() {
	return orig_timestamp2;
}

alt_u32 NTPDatagram::getRecvTimestamp1() {
	return recv_timestamp1;
}

alt_u32 NTPDatagram::getRecvTimestamp2() {
	return recv_timestamp2;
}

alt_u32 NTPDatagram::getTransTimestamp1() {
	return trans_timestamp1;
}

alt_u32 NTPDatagram::getTransTimestamp2() {
	return trans_timestamp2;
}

alt_u32 NTPDatagram::getKeyId() {
	return key_id;
}

alt_u32 NTPDatagram::getMsgDigest1() {
	return msg_digest1;
}

alt_u32 NTPDatagram::getMsgDigest2() {
	return msg_digest2;
}

alt_u32 NTPDatagram::getMsgDigest3() {
	return msg_digest3;
}

alt_u32 NTPDatagram::getMsgDigest4() {
	return msg_digest4;
}

int NTPDatagram::getStream(char** buffer) {
	if (*buffer == NULL) return 0;
	*buffer = (char*)stream;
	return NTP_BUFFER_SIZE;
}

struct tm * NTPDatagram::ntp2unix(u_long ntp) {
    time_t t, curr;
    struct tm *tm;
    int curr_year, epoch_nr;

    /* First get the current year: */
    time(&curr);
    tm = gmtime(&curr);
    if (!tm) return NULL;

    curr_year = 1900 + tm->tm_year;

    /* Convert the ntp timestamp to a unix utc seconds count: */
    t = (time_t) ntp - JAN_1970;

    /* Check that the ntp timestamp is not before a 136 year window centered
       around the current year:

       Failsafe in case of an infinite loop:
       Allow up to 1000 epochs of 136 years each!
    */
    for (epoch_nr = 0; epoch_nr < MAX_EPOCH_NR; epoch_nr++) {
        tm = gmtime(&t);

#if SIZEOF_TIME_T == 4
        /* If 32 bits, then year is 1970-2038, so no sense looking */
        epoch_nr = MAX_EPOCH_NR;
#else    /* SIZEOF_TIME_T > 4 */
        /* Check that the resulting year is in the correct epoch: */
        if (1900 + tm->tm_year > curr_year - 68) break;

        /* Epoch wraparound: Add 2^32 seconds! */
        t += (time_t) 65536 << 16;
#endif /* SIZEOF_TIME_T > 4 */
    }
    return tm;
}
