#ifndef STFTCOMPUTETHREAD_H
#define STFTCOMPUTETHREAD_H

#include <QThread>
#include <QMutex>

#include "sigproc.h"

class FTSound;

class STFTComputeThread : public QThread
{
    Q_OBJECT

    sigproc::FFTwrapper* m_fft;   // The FFT transformer

    void run(); //Q_DECL_OVERRIDE

signals:
    void stftComputing();
    void stftFinished(bool canceled=false);
    void stftProgressing(int);

public:
    STFTComputeThread(QObject* parent);

    class Parameters{
    public:
        FTSound* snd;
        qreal ampscale; // [linear]
        qint64 delay;   // [sample index]

        std::vector<FFTTYPE> win;
        int stepsize;
        int dftlen;

        int cepliftorder;
        bool cepliftpresdc;

        void clear(){
            snd = NULL;
            ampscale = 1.0;
            delay = 0;
            win.clear();
            stepsize = -1;
            dftlen = -1;
            cepliftorder = -1;
            cepliftpresdc = false;
        }

        Parameters(){
            clear();
        }
        Parameters(FTSound* reqnd, const std::vector<FFTTYPE>& reqwin, int reqstepsize, int reqdftlen, int reqcepliftorder, bool reqcepliftpresdc);

        bool operator==(const Parameters& param);
        bool operator!=(const Parameters& param){
            return !((*this)==param);
        }

        inline bool isEmpty(){return snd==NULL;}
    };

    void compute(FTSound* snd, const std::vector<FFTTYPE>& win, int stepsize, int dftlen, int cepliftorder, bool cepliftpresdc);     // Entry point

    mutable QMutex m_mutex_computing;      // To protect the access to the FFT and external variables
    mutable QMutex m_mutex_changingparams; // To protect the access to the parameters above

    inline const Parameters& getCurrentParameters() const {return m_params_current;}

private:
    Parameters m_params_current;   // The params which are in preparation by the thread
    Parameters m_params_todo;      // The params which have to be done by the thread
};

#endif // STFTCOMPUTETHREAD_H