#include "U2F_Authenticate_APDU.hpp"
#include "Field.hpp"
#include "U2FMessage.hpp"
#include "u2f.hpp"
#include "Field.tpp"
#include "APDU.hpp"
#include <iostream>
#include "Signature.hpp"
#include "micro-ecc/uECC.h"
#include <mbedtls/sha256.h>

using namespace std;

U2F_Authenticate_APDU::U2F_Authenticate_APDU(const U2F_Msg_CMD &msg, const vector<uint8_t> &data)
	: U2F_Msg_CMD{ msg }
{
	if (p2 != 0)
	{
		//Invalid U2F (APDU) parameter detected
		throw APDU_STATUS::SW_CONDITIONS_NOT_SATISFIED;
	}
	else if (data.size() < 66)
	{
		//Invalid authentication request
		throw APDU_STATUS::SW_WRONG_LENGTH;
	}

	copy(data.begin() + 0,  data.begin() + 32, challengeP.begin());
	copy(data.begin() + 32, data.begin() + 64, appParam.begin());

	uint8_t keyHLen = data[64];

	copy(data.begin() + 65, data.begin() + 65 + keyHLen, back_inserter(keyH));
}

void U2F_Authenticate_APDU::respond(const uint32_t channelID) const
{
	if (keyH.size() != sizeof(Storage::KeyHandle))
	{
		//Respond with error code - key handle is of wrong size
		cerr << "Invalid key handle length" << endl;
		this->error(channelID, APDU_STATUS::SW_WRONG_DATA);
		return;
	}

	auto keyHB = *reinterpret_cast<const Storage::KeyHandle*>(keyH.data());
	
	if (Storage::appParams.find(keyHB) == Storage::appParams.end())
	{
		//Respond with error code - key handle doesn't exist in storage
		cerr << "Invalid key handle" << endl;
		this->error(channelID, SW_WRONG_DATA);
		return;
	}

	auto appMatches = (Storage::appParams.at(keyHB) == appParam);

	U2FMessage msg{};
	msg.cid = channelID;
	msg.cmd = U2FHID_MSG;

	auto &response = msg.data;
	APDU_STATUS statusCode = APDU_STATUS::SW_NO_ERROR;

	switch (p1)
	{
		case ControlCode::CheckOnly:
			if (appMatches)
				statusCode = APDU_STATUS::SW_CONDITIONS_NOT_SATISFIED;
			else
				statusCode = APDU_STATUS::SW_WRONG_DATA;

			response.insert(response.end(), FIELD_BE(statusCode));
			msg.write();
			return;
		case ControlCode::EnforcePresenceSign:
			//Continue processing
		case ControlCode::DontEnforcePresenceSign:
			//Continue processing
			break;

		default:
			cerr << "Unknown APDU authentication command" << endl;
			this->error(channelID, APDU_STATUS::SW_INS_NOT_SUPPORTED);
			return;
	}

	const auto& privKey = Storage::privKeys[keyHB];
	auto& keyCount      = Storage::keyCounts[keyHB];
	keyCount++;

	response.push_back(0x01);
	response.insert(response.end(), FIELD_BE(keyCount));

	Digest digest;
	{
		mbedtls_sha256_context shaContext;

		mbedtls_sha256_init(&shaContext);
		mbedtls_sha256_starts(&shaContext, 0);

		mbedtls_sha256_update(&shaContext, reinterpret_cast<const uint8_t *>(appParam.data()), sizeof(appParam));
		uint8_t userPresence{ 1u };
		mbedtls_sha256_update(&shaContext, &userPresence, 1);
		const auto beCounter = beEncode(keyCount);
		mbedtls_sha256_update(&shaContext, beCounter.data(), beCounter.size());
		mbedtls_sha256_update(&shaContext, challengeP.data(), challengeP.size());

		mbedtls_sha256_finish(&shaContext, digest.data());
		mbedtls_sha256_free(&shaContext);
	}

	Signature signature{};
	uECC_sign(privKey.data(), digest.data(), digest.size(), signature.data(), uECC_secp256r1());

	appendSignatureAsDER(response, signature);
	response.insert(response.end(), FIELD_BE(statusCode));

	msg.write();
}