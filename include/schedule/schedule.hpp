#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

// dosage from each container
typedef struct {
    int dose;
    int times_count;
    int times[8];
} container_t;

// all dosing times
typedef struct {
    container_t containers[5];
} daily_schedule_t;

class Schedule{
    public:
        Schedule(){}
        Schedule(daily_schedule_t _schedule): schedule(_schedule) {}
        daily_schedule_t get_schedule() { return schedule; }
        void set_schedule(daily_schedule_t _schedule) { schedule = _schedule; };
    protected:
        daily_schedule_t schedule;
};

#endif