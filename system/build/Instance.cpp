#include "Instance.h"

#include <QDir>
#include "Defines.h"

static Instance _instance;

Instance::Instance()
{
}

Instance::~Instance()
{
    delete m_pConfig;
}

void Instance::init()
{
    m_pConfig = new Config();
    m_pConfig->init (CONF);
}

Instance* Instance::instance()
{
    return &_instance;
}
