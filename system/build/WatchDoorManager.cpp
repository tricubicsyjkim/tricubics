#include "WatchDoorManager.h"
#include <assert.h>

WatchDoorManager::WatchDoorManager()
{
//    context = zmq_ctx_new ();
//    sender = zmq_socket (context, ZMQ_PUSH);
//    QString sender_addr = "tcp://0.0.0.0:3001";
//    if(sender_addr != NULL){
//        zmq_connect(sender, sender_addr.toStdString().data());
//    }

    receiver = zmq_socket (context, ZMQ_PULL);
    QString receiver_addr = "tcp://0.0.0.0:1234";
    if(receiver_addr != NULL){
        zmq_connect(receiver, receiver_addr.toStdString().data());
    }

    qDebug("end init");
}

WatchDoorManager::~WatchDoorManager()
{
    if(receiver != nullptr){
        zmq_close (receiver);
    }
    zmq_ctx_destroy (context);
}

bool WatchDoorManager::proc()
{
    void *context = zmq_ctx_new ();
      void *responder = zmq_socket (context, ZMQ_REP);
      int rc = zmq_bind (responder, "tcp://*:9999");
      assert(rc == 0);

      while (1) {
          char buffer [10];
          zmq_recv (responder, buffer, 10, 0);
        //  sleep (1);          //  Do some 'work'
          zmq_send (responder, "World", 5, 0);
         emit watch_door();
      }
}



void WatchDoorManager::run()
{
    qDebug()<<"WatchDoorManager run";
   // proc();
}
