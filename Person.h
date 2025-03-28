
#define PERSON_H

class Job;
class WorkStation;

class Person {
    public:
        Person(Job* job, double lastArrivalTime = 0.0, int id = 0);
        Job* getJob() const;
        void setJob(Job* job);
        int getJobDone() const;
        void setJobDone(int count);
        double getEventTime() const;
        void setEventTime(double time);
        int getID() const;
        double getLastArrivalTime() const;
        void setLastArrivalTime(double time);
        WorkStation* getNext();
        WorkStation* getCurrent();
    
    private:
        int id_;
        Job* job_;
        int jobDone_;
        double eventTime_;
        double lastArrivalTime_;
    };
    

