/* Calf DSP Library
 * Utilities
 *
 * Copyright (C) 2008 Krzysztof Foltman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __CALF_UTILS_H
#define __CALF_UTILS_H

#include <errno.h>
#include <map>
#include <string>

#ifndef _WIN32
#include <pthread.h>
#else
#include <QMutex>
#endif

namespace calf_utils
{

#ifdef _WIN32
/* Mutex types.  */
enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
};
#endif

/// Pthreads based mutex class
class ptmutex
{
public:
#ifndef _WIN32
    pthread_mutex_t pm;
#else
    QMutex qm;
#endif
    
    ptmutex(int type = PTHREAD_MUTEX_RECURSIVE)
#ifdef _WIN32
        : qm ((type == PTHREAD_MUTEX_RECURSIVE) ? QMutex::Recursive : QMutex::NonRecursive)
#endif
    {
#ifndef _WIN32
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, type);
        pthread_mutex_init(&pm, &attr);
        pthread_mutexattr_destroy(&attr);
#endif
    }
    
    bool lock()
    {
#ifdef _WIN32
        qm.lock();
        return true;
#else
        return pthread_mutex_lock(&pm) == 0;
#endif
    }
    
    bool trylock()
    {
#ifdef _WIN32
        return qm.tryLock();
#else
        return pthread_mutex_trylock(&pm) == 0;
#endif
    }
    
    void unlock()
    {
#ifdef _WIN32
        qm.unlock();
#else
        pthread_mutex_unlock(&pm);
#endif
    }

    ~ptmutex()
    {
#ifndef _WIN32
        pthread_mutex_destroy(&pm);
#endif
    }
};


/// Exception-safe mutex lock
class ptlock
{
    ptmutex &mutex;
    bool locked;
    
public:
    ptlock(ptmutex &_m) : mutex(_m), locked(true)
    {
        mutex.lock();
    }
    void unlock()
    {
        mutex.unlock();
        locked = false;
    }
    void unlocked()
    {
        locked = false;
    }
    ~ptlock()
    {
        if (locked)
            mutex.unlock();
    }
};

/// Exception-safe temporary assignment
template<class T>
class scope_assign
{
    T &data, old_value;
public:
    scope_assign(T &_data, T new_value)
    : data(_data), old_value(_data)
    {
        data = new_value;
    }
    ~scope_assign()
    {
        data = old_value;
    }
};

struct text_exception: public std::exception
{
    const char *text;
    std::string container;
public:
    text_exception(const std::string &t) : container(t) { text = container.c_str(); }
    virtual const char *what() const throw () { return text; }
    virtual ~text_exception() throw () {}
};

struct file_exception: public std::exception
{
    const char *text;
    std::string message, filename, container;
public:
    file_exception(const std::string &f) : message(strerror(errno)), filename(f), container(filename + ":" + message) { text = container.c_str(); }
    file_exception(const std::string &f, const std::string &t) : message(t), filename(f), container(filename + ":" + message) { text = container.c_str(); }
    virtual const char *what() const throw () { return text; }
    virtual ~file_exception() throw () {}
};

/// String-to-string mapping
typedef std::map<std::string, std::string> dictionary;

/// Serialize a dictonary to a string
extern std::string encode_map(const dictionary &data);
/// Deserialize a dictonary from a string
extern void decode_map(dictionary &data, const std::string &src);

/// int-to-string
extern std::string i2s(int value);

/// float-to-string
extern std::string f2s(double value);

/// Escape a string to be used in XML file
std::string xml_escape(const std::string &src);

/// Load file from disk into a std::string blob, or throw file_exception
std::string load_file(const std::string &src);

/// Indent a string by another string (prefix each line)
std::string indent(const std::string &src, const std::string &indent);

};

#endif
