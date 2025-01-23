#ifndef CSINGLETONE
#define CSINGLETONE

template <typename T>
class CSingleTon
{
private:
    static T * pInstance;

public:
    static T& getInstance()
    {
        if(pInstance == NULL) pInstance = new T;
        return *pInstance;
    }

    static T* getInstancePtr()
    {
        if(pInstance == NULL) pInstance = new T;
        return pInstance;
    }

    static void releaseInstance()
    {
        if(pInstance != NULL) delete pInstance;
    }

};

#endif // CSINGLETONE

