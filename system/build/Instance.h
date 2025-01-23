#ifndef INSTANCE_H
#define INSTANCE_H

#include "Defines.h"
#include "Config.h"

#define _CONF Instance::instance()->config()

class Instance
{
public:
    explicit Instance();
    ~Instance();

    void init();
    Config* config() { return m_pConfig; }

    static Instance* instance();

public:
    Config *m_pConfig;
};

#endif // INSTANCE_H
