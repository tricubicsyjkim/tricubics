#include "SensorDAQImpl.h"

#include <functional>
#include <numeric>

SensorDAQImpl::SensorDAQImpl(QObject *parent) : QObject(parent) 
{
    connect(this,
            SIGNAL(value_of_steady_state(const int &, const int &)),
            this,
            SLOT(on_delta_triggered(const int &, const int &)),
            Qt::QueuedConnection);

    connect(this,
            SIGNAL(value_of_steady_state(const int &, const int &, const int &)),
            this,
            SLOT(on_delta_triggered(const int &, const int &, const int &)),
            Qt::QueuedConnection);
    //snack shelf
    connect(this,
            SIGNAL(value_of_steady_state(const int &, const int &, const int &, const int &)),  //mt_index, shelf_no, weight_index, delta
            this,
            SLOT(on_delta_triggered(const int &, const int &, const int &, const int &)),   //mt_index, shelf_no, weight_index, delta
            Qt::QueuedConnection);
}

int SensorDAQImpl::get_smoothed_value(int id, int value) 
{
    daq_data_[id].smooth_queue.push_back(value);

    if (daq_data_[id].smooth_queue.size() <= static_cast<unsigned>(daq_data_[id].threshold_smooth_queue_size))
        return value;

    daq_data_[id].smooth_queue.pop_front();

    return static_cast<int>(std::accumulate(daq_data_[id].smooth_queue.begin(),daq_data_[id].smooth_queue.end(), 0.0)
                            / static_cast<double>(daq_data_[id].smooth_queue.size()));
}

daq_data *SensorDAQImpl::get_sensor_ptr(int id) 
{
    return (daq_data_.count(id)) ? &daq_data_[id] : nullptr;
}

void SensorDAQImpl::add_sensor(int id, int type, int noise, int settling_cnts, int smooth_queuesize) 
{
    daq_data dd;
    dd.acq_type = type;
    dd.threshold_noise = noise;
    dd.threshold_settling_counts = settling_cnts;
    dd.threshold_smooth_queue_size = smooth_queuesize;

    daq_data_[id] = dd;
}

void SensorDAQImpl::updateData(int id, int value) 
{
    if (daq_data_[id].last_value == 0 && daq_data_[id].last_stable_value == 0) {
        daq_data_[id].last_value = daq_data_[id].last_stable_value = value;
        return;
    }

   daq_data_[id].capture_value = value;
#if 1
    int smoothed_value = (daq_data_[id].threshold_smooth_queue_size == 0) ? value: get_smoothed_value(id, value);

    if (daq_data_[id].last_value - daq_data_[id].threshold_noise < smoothed_value &&
            daq_data_[id].last_value + daq_data_[id].threshold_noise > smoothed_value)
        daq_data_[id].settling_counts++;
    else
        daq_data_[id].settling_counts = 0;
    daq_data_[id].last_value = smoothed_value;
    if( daq_data_[id].settling_counts >= daq_data_[id].threshold_settling_counts) {
        int delta = smoothed_value - daq_data_[id].last_stable_value;
        if (abs(delta) > 200){//daq_data_[id].threshold_noise) {
            if (daq_data_[id].acq_type == DAQ_AUTO)
                emit value_of_steady_state(id, delta);
            daq_data_[id].last_stable_value = smoothed_value;
        }
        daq_data_[id].settling_counts = 0;
    }

#else

    //does not use average weight value
    daq_data_[id].last_value = daq_data_[id].last_stable_value = value;

#endif

}
