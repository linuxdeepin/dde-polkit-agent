#ifndef AGENTEXTENSIONPROXY_H
#define AGENTEXTENSIONPROXY_H

namespace dpa {

class AgentExtensionProxy {
public:
    /**
     * @brief actionID returns the ID of the authenticated action.
     */
    virtual const QString & actionID() const = 0;

    /**
     * @brief username returns the name of the user authenticated.
     */
    virtual const QString & username() const = 0;

    /**
     * @brief password returns the password the user typed.
     */
    virtual const QString & password() const = 0;
};

}

#endif // AGENTEXTENSIONPROXY_H
