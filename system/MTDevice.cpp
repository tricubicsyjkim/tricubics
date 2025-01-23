#include "MTDevice.h"
#include "Instance.h"
#include <QDebug>
#include <mtdev.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

typedef __u64 mstime_t;

MTDevice::MTDevice()
{
    //some connect
    fd = open(_CONF->mt_device_.toLocal8Bit().data(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        qDebug("error: could not open device");
        return;
    }else{
        qDebug("open mt device");
    }

}

MTDevice::MTDevice(QString mtdev_path)
{
    qDebug()<<"mtdev_path"<<mtdev_path;
    fd = open(mtdev_path.toLocal8Bit().data(), O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        qDebug("error: could not open device");
        return;
    }else{
        qDebug("open mt device");
    }
}

MTDevice::~MTDevice()
{
    qDebug("close mt dev");
    close(fd);
}

void MTDevice::stop()
{
    close(fd);
}

bool MTDevice::proc()
{

#if 0
    struct mtdev dev;
    struct input_event ev;
    int ret = mtdev_open(&dev, fd);
    if (ret) {
        qDebug( "error: could not open device: %d", ret);
    }
#ifdef DEBUG
    qDebug("mt manager start");
#endif
    while(!mtdev_idle(&dev, fd, -1)){
        while (mtdev_get(&dev, fd, &ev, 1) > 0) {
            //print_event(&ev);
            //analysis_event(&ev);
        }
    }

#else

    while(1){
        usleep(1);
    }
#endif
    return true;
}

int MTDevice::mtdev_get(mtdev *dev, int fd, input_event *ev, int ev_max)
{
    struct input_event kev;
    int ret, count = 0;
    while (count < ev_max) {

        mtdev_slot  mtdev_data;
        while (mtdev_empty(dev)){
            usleep(1);
            ret = mtdev_fetch_event(dev, fd, &kev);
            if (ret <= 0)
                return count > 0 ? count : ret;
            mtdev_put_event(dev, &kev);
            analysis_event(&kev, mtdev_data);
#ifdef DEBUG
            qDebug("\n");
#endif
        }
#if 0
        while(!mtdev_empty(dev)){
            mtdev_get_event(dev, &ev[count++]);
            print_event(ev);
        }

#else
        mtdev_get_event(dev, &ev[count++]);

#endif
    }
    return count;
}

int MTDevice::mtdev_empty(mtdev *dev)
{
    return evbuf_empty(&dev->state->outbuf);
}

int MTDevice::mtdev_fetch_event(mtdev *dev, int fd, input_event *ev)
{
    struct mtdev_iobuf *buf = &dev->state->iobuf;
    int n = buf->head - buf->tail;  // n = 0, 24, 48, 72, 96 .......
    // event size = 24
    if (n < (int)EVENT_SIZE) {
        if (buf->tail && n > 0)
            memmove(buf->data, buf->data + buf->tail, n);
        buf->head = n;
        buf->tail = 0;
        n = read(fd, buf->data + buf->head,
                 DIM_BUFFER - buf->head);
        if (n <= 0)
            return n;

        buf->head += n;
    }
    if (buf->head - buf->tail < (int)EVENT_SIZE)
        return 0;
    memcpy(ev, buf->data + buf->tail, EVENT_SIZE);
    buf->tail += EVENT_SIZE;
    return 1;
}

void MTDevice::mtdev_put_event(mtdev *dev, const input_event *ev)
{
    struct mtdev_state *state = dev->state;
    if (ev->type == EV_SYN && ev->code == SYN_REPORT) {
        unsigned int head = state->outbuf.head;    //head= 0, 9, 14, 21 ......   how many events get in.
        if (mtdev_has_mt_event(dev, ABS_MT_SLOT))
            process_typeB(state);
        if (state->outbuf.head != (int)head)
            evbuf_put(&state->outbuf, ev);
        // if event is EV_SYN put data into outbuf, other events put into inbuf
    } else {
        evbuf_put(&state->inbuf, ev);
    }
}

void MTDevice::mtdev_get_event(mtdev *dev, input_event *ev)
{
    evbuf_get(&dev->state->outbuf, ev);
}

void MTDevice::process_typeB(mtdev_state *state)
{
    struct input_event ev;
    while (!evbuf_empty(&state->inbuf)) {
        evbuf_get(&state->inbuf, &ev);
        evbuf_put(&state->outbuf, &ev);
    }
}

int MTDevice::evbuf_empty(const mtdev_evbuf *evbuf)
{
    return evbuf->head == evbuf->tail;
}

void MTDevice::evbuf_put(mtdev_evbuf *evbuf, const input_event *ev)
{
    evbuf->buffer[evbuf->head++] = *ev;
    evbuf->head &= DIM_EVENTS - 1;  // why does &= ???
}

void MTDevice::evbuf_get(mtdev_evbuf *evbuf, input_event *ev)
{
    *ev = evbuf->buffer[evbuf->tail++];
    evbuf->tail &= DIM_EVENTS - 1;
}

void MTDevice::print_event(const input_event *ev)
{
    static const mstime_t ms = 1000;
    static int slot;
    mstime_t evtime = ev->time.tv_usec / ms + ev->time.tv_sec * ms;
    if (ev->type == EV_ABS && ev->code == ABS_MT_SLOT)
        slot = ev->value;
    fprintf(stderr, "%012llx %02d %01d %04x %d\n",
            evtime, slot, ev->type, ev->code, ev->value);
}

void MTDevice::analysis_event(const input_event *ev, mtdev_slot &mtdev_data)
{
    switch(ev->type){
    case EV_ABS:
        switch (ev->code) {
        case ABS_X:
            //qDebug("ABS_X:%d \n ", ev->value);
            break;
        case ABS_Y	:
            //qDebug("ABS_Y:%d \n ", ev->value);
            break;
        case ABS_MT_SLOT:
            //qDebug("ABS_MT_SLOT:%d \n ", ev->value);
            break;
        case ABS_MT_TOUCH_MAJOR:
            //qDebug("ABS_MT_TOUCH_MAJOR:%d \n ", ev->value);
            mtdev_data.touch_major = ev->value;
            break;
        case ABS_MT_TOUCH_MINOR:
            //qDebug("ABS_MT_TOUCH_MINOR:%d \n ", ev->value);
            mtdev_data.touch_minor = ev->value;
            break;
        case ABS_MT_WIDTH_MAJOR:
            //qDebug("ABS_MT_WIDTH_MAJOR:%d \n ", ev->value);
            mtdev_data.width_major = ev->value;
            break;
        case ABS_MT_WIDTH_MINOR:
            //qDebug("ABS_MT_WIDTH_MINOR:%d \n ", ev->value);
            mtdev_data.width_minor = ev->value;
            break;
        case ABS_MT_POSITION_X:
#ifdef DEBUG
            qDebug("ABS_MT_POSITION_X:%d", ev->value);
#endif
            mtdev_data.position_x = ev->value;
            check_x_value((int)ev->value);
            break;
        case ABS_MT_POSITION_Y:
#ifdef DEBUG
            qDebug("ABS_MT_POSITION_Y:%d", ev->value);
#endif
            mtdev_data.position_y = ev->value;
            break;
        case ABS_MT_TRACKING_ID:
            //qDebug("ABS_MT_TRACKING_ID:%d \n ", ev->value);
            mtdev_data.tracking_id = (int)ev->value;
            break;
        default:
            //qDebug("ABS code :%d, value: %d \n ", ev->code, ev->value);
            break;
        }
        break;
    case EV_SYN:
        switch (ev->code) {
        case SYN_REPORT:
            //qDebug("SYN_REPORT:%d \n\n ", ev->value);
            break;
        case SYN_MT_REPORT:
            //qDebug("SYN_MT_REPORT:%d \n ", ev->value);
            break;
        case SYN_CONFIG:
            //qDebug("SYN_CONFIG:%d \n ", ev->value);
            break;
        default:
            //qDebug("syn code:%d, value: %d \n ",ev->code, ev->value);
            break;
        }

        break;

    case EV_KEY:
        switch (ev->code) {
        case KEY_RESERVED:
            //qDebug("KEY_RESERVED:%d \n ", ev->value);
            break;
        case KEY_ESC:
            //qDebug("KEY_ESC:%d \n ", ev->value);
            break;
        case BTN_TOUCH:
            if(0 == (int)ev->value){
#ifdef DEBUG
                qDebug()<<"send hands_out_process ";
#endif
                emit hands_out_process();
            }
#ifdef DEBUG
            qDebug("BTN_TOUCH:%d \n ", ev->value);
#endif
            break;
        default:
            //qDebug("key code:%d, value: %d \n ",ev->code, ev->value);
            break;
        }
        break;
    default:
        //qDebug("event type:%d, code: %d, value: %d \n ", ev->type, ev->code, ev->value);
        break;
    }
}

void MTDevice::check_x_value(int x)
{
    int shelf_id =  get_shelf_id_from_vec(x);
    if(shelf_id != 0){
#ifdef DEBUG
        qDebug()<<"hand_in_process"<<"  , shelf: "<<shelf_id;
#endif
        emit hand_in_process(shelf_id);
    }
}

int MTDevice::get_shelf_id(int x)
{
    if(x <=  shelf_roi[0] && x>=shelf_roi[1] ){
        return 1;
    }else if(x <=  shelf_roi[1] && x>=shelf_roi[2]){
        return 2;
    }else if(x <=  shelf_roi[2] && x>=shelf_roi[3]){
        return 3;
    }else if(x <=  shelf_roi[3] && x>=shelf_roi[4]){
        return 4;
    }else if(x <=  shelf_roi[4] && x>=shelf_roi[5]){
        return 5;
    }else if(x <=  shelf_roi[5] && x>=shelf_roi[6]){
        return 6;
    }else{
        return 0;
    }
}

int MTDevice::get_shelf_id_from_vec(int x)
{
    if(x <=  MAX_MT_VALUE && x>=  _CONF->mt_x_values[0] ){
        return 1;
    }else if(x <=   _CONF->mt_x_values[0] && x>=  _CONF->mt_x_values[1]){
        return 2;
    }else if(x <=  _CONF->mt_x_values[1] && x>=  _CONF->mt_x_values[2]){
        return 3;
    }else if(x <=   _CONF->mt_x_values[2] && x>=  _CONF->mt_x_values[3]){
        return 4;
    }else if(x <=   _CONF->mt_x_values[3] && x>=  _CONF->mt_x_values[4]){
        return 5;
    }else if(x <=   _CONF->mt_x_values[4] && x>=  _CONF->mt_x_values[5]){
        return 6;
    }else{
        return 0;
    }
}

void MTDevice::closeMTDev()
{

}

void MTDevice::run()
{
#ifdef DEBUG
    qDebug("mtdev run");
#endif
    this->proc();
}


