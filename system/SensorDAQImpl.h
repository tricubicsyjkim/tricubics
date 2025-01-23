#ifndef SENSORDAQIMPL_H
#define SENSORDAQIMPL_H

#include <QObject>
#include <QMap>

#include <list>
#include <vector>
#include <string>
#include <algorithm>

#define CHAR_STX 2
#define CHAR_ETX 3

enum {
    DAQ_MANUAL,
    DAQ_AUTO,
};

struct daq_data {
    int acq_type;
    int threshold_noise, threshold_settling_counts, threshold_smooth_queue_size;
    int settling_counts;
    int last_stable_value, last_value, value;
    int capture_value;

    //if photo sensor is on ,   count_trigger++, if   photo sensor is off, count_trigger
    int count_trigger;
    bool is_on;
    std::list<int> smooth_queue;
    daq_data():
        acq_type(DAQ_MANUAL), settling_counts(0), last_stable_value(0), last_value(0), capture_value(0), count_trigger(0), is_on(false) {}
};

class SensorDAQImpl : public QObject 
{
    Q_OBJECT
public:
    explicit SensorDAQImpl(QObject *parent = nullptr);

    void add_sensor(int id, int acq_type, int noise, int settling_cnts, int smooth_queue_size);
    virtual void updateData(int id, int value);
    int get_smoothed_value(int id, int value);

    daq_data *get_sensor_ptr(int id);

signals:
    void value_of_steady_state(int, int);
    void value_of_steady_state(int, int, int);  // mt_index , shelf_id , weight value
    //snack shelf
    void value_of_steady_state(int, int, int, int);  // mt_index , shelf_id , weight index, weight value

public slots:
    virtual void on_delta_triggered(int, int) = 0;
    virtual void on_delta_triggered(int, int, int) = 0;
    //snack shelf
    virtual void on_delta_triggered(int, int, int, int) = 0;
protected:
    QMap<int, daq_data> daq_data_;
};

#endif // SENSORDAQIMPL_H
