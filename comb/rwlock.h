#include <atomic>
#include <condition_variable>
#include <mutex>

class CRWMtx {
public:
    CRWMtx()
    {
        m_WrCount = 0;
        m_RdCount = 0;
        m_bWriteFinished = true;
    }

    ~CRWMtx() {}

    void RLock()
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        m_rd_cv.wait(lk, [this] { return m_bWriteFinished && (m_WrCount == 0); });
        ++m_RdCount;
    }

    void RUnLock()
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        --m_RdCount;
        m_rd_cv.notify_all();
    }

    void WLock()
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        m_bWriteFinished = false;
        m_rd_cv.wait(lk, [this] { return (m_RdCount <= 0 && m_WrCount == 0); });
        ++m_WrCount;
    }

    void WUnLock()
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        if (--m_WrCount == 0) {
            m_bWriteFinished = true;
        }
        m_rd_cv.notify_all();
    }

private:
    std::atomic_bool m_bWriteFinished;
    std::atomic<unsigned long long> m_WrCount;
    std::atomic<unsigned long long> m_RdCount;
    std::mutex m_mtx;
    std::condition_variable m_rd_cv;
};

class CReadLock {
public:
    CReadLock(CRWMtx &rwMtx) : m_rwMtx(rwMtx) { m_rwMtx.RLock(); }
    ~CReadLock() { m_rwMtx.RUnLock(); }

private:
    CRWMtx &m_rwMtx;
};

class CWriteLock {
public:
    CWriteLock(CRWMtx &rwMtx) : m_rwMtx(rwMtx) { m_rwMtx.WLock(); }
    ~CWriteLock() { m_rwMtx.WUnLock(); }

private:
    CRWMtx &m_rwMtx;
};