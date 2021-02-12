#ifndef NAMINUKAS_BRAIN_ACTION_ACTION_H_
#define NAMINUKAS_BRAIN_ACTION_ACTION_H_

class Action {
  public:
    virtual void start();
    virtual void abort();

    /**
     * Return true if action is complete.
     */
    virtual bool execute();
};

#endif  // NAMINUKAS_BRAIN_ACTION_ACTION_H_
