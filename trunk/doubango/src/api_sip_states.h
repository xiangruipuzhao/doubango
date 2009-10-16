#ifndef _DOUBANGO_SIP_STATES_H_
#define _DOUBANGO_SIP_STATES_H_

#include "pref.h"

PREF_NAMESPACE_START

typedef enum tag_SIP_STATE
{
	SS_UNKNOWN_UNKNOWN,

	/* REGISTER */
	SS_REGISTER_UNKNOWN,
	SS_REGISTER_INITIALIZED,
	SS_REGISTER_TRYING,
	SS_REGISTER_AUTHENTIFYING,
	SS_REGISTER_ESTABLISHED,
	SS_REGISTER_TERMINATED,

	/* PUBLISH */
	SS_PUBLISH_UNKNOWN,
	SS_PUBLISH_INITIALIZED,
	SS_PUBLISH_TRYING,
	SS_PUBLISH_AUTHENTIFYING,
	SS_PUBLISH_ESTABLISHED,
	SS_PUBLISH_TERMINATED,

	/* SUBSCRIBE */
	SS_SUBSCRIBE_UNKNOWN,
	SS_SUBSCRIBE_INITIALIZED,
	SS_SUBSCRIBE_TRYING,
	SS_SUBSCRIBE_AUTHENTIFYING,
	SS_SUBSCRIBE_ESTABLISHED,
	SS_SUBSCRIBE_NOTIFIED,
	SS_SUBSCRIBE_TERMINATED,

	/* MESSAGE */
	SS_MESSAGE_UNKNOWN,
	SS_MESSAGE_INITIALIZED,
	SS_MESSAGE_TRYING,
	SS_MESSAGE_AUTHENTIFYING,
	SS_MESSAGE_TERMINATED,
}
SIP_STATE;

PREF_NAMESPACE_END

#endif /* _DOUBANGO_SIP_STATES_H_ */