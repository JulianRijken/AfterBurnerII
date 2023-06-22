#pragma once
#include <functional>
#include <memory>
#include <unordered_set>
#include <format>

#include "Component.h"
#include "Log.h"

class Component;

class Time
{
    friend class Game;

public:

    class Timer
    {
        friend Time;

    public:
        Timer(float delay, bool looping, bool ignoreTimeScale, std::function<void()> callBack) :
            m_CallBack{ std::move(callBack) },
            m_ElapsedTime{ 0 },
            m_Delay{ std::max(0.0f,delay)},
            m_Looping{ looping },
            m_IgnoreTimeScale{ignoreTimeScale},
            m_Paused{false},
            m_Cancelled{false},
            m_TimesLooped{0}
        {}

        void Cancel()
        {
            m_Cancelled = true;
        }

        void SetPaused(bool paused)
        {
            m_Paused = paused;
        }

        void SetDelay(float delay)
        {
            m_Delay = std::max(0.0f, delay);
        }

        void Reset()
        {
            m_ElapsedTime = 0.0f;
            m_TimesLooped = 0;
        }

        [[nodiscard]] int GetTimesLooped() const {return m_TimesLooped;}

        [[nodiscard]] float GetElapsedTime() const{return m_ElapsedTime;}

        void Update()
        {

            // The update should not be called once cancelled but do check to be safe 

            if (m_Paused || m_Cancelled)
                return;


            // Increment time
            if(m_IgnoreTimeScale)
				m_ElapsedTime += m_DeltaTime;
            else
                m_ElapsedTime += m_DeltaTime * m_TimeScale;

            // Check if elapsed time is more then delay
            if (m_ElapsedTime < m_Delay)
                return;

            if(m_CallBack)
				m_CallBack();

            if (not m_Looping)
            {
                m_Cancelled = true;
                return;
            }

            m_TimesLooped++;
            m_ElapsedTime -= m_Delay;
        }

    private:
        std::function<void()> m_CallBack;
        float m_ElapsedTime;
        float m_Delay;
        bool m_Looping;
        bool m_IgnoreTimeScale;
        bool m_Paused;
        bool m_Cancelled;
        int m_TimesLooped;
    };

    template<typename ObjectType>
    static std::shared_ptr<Timer> StartTimer(std::weak_ptr<ObjectType> objectPtr, bool ignoreTimeScale = false)
    {
        const auto timer{ std::make_shared<Timer>(0.0f,true,ignoreTimeScale,nullptr) };
        const std::weak_ptr weakTimer{ timer };
        m_TimerPtrs.insert(timer);
        return timer;
    }

    template<typename ObjectType>
    static std::shared_ptr<Timer> StartTimer(float delay, bool looping, std::weak_ptr<ObjectType> objectPtr, bool ignoreTimeScale = false)
    {
        const std::shared_ptr timerSPtr{ std::make_shared<Timer>(delay,looping,ignoreTimeScale,nullptr) };
        const std::weak_ptr timerWPtr{ timerSPtr };
        m_TimerPtrs.insert(timerSPtr);
        return timerSPtr;
    }


    // Starts a internal timer that is updated before the normal update loop.
    // Will be destroyed when object is destroyed
    // Allowing for a callback with (Time::Timer* + optional Args) or (Args)


    template<typename Function, typename ObjectType, typename... Args>
    static std::shared_ptr<Timer> StartTimer(float delay, bool looping, std::weak_ptr<ObjectType> objectPtr, Function&& function, Args&&... args)
	{
        return StartTimer(delay,looping,false, objectPtr, function, args...);
    }

    template<typename Function, typename ObjectType, typename... Args>
    static std::shared_ptr<Timer> StartTimer(float delay, bool looping, bool ignoreTimeScale, std::weak_ptr<ObjectType> objectPtr, Function&& function, Args&&... args)
    {
        const std::shared_ptr timerSPtr{ std::make_shared<Timer>(delay,looping,ignoreTimeScale,nullptr) };
        const std::weak_ptr timerWPtr{ timerSPtr };

        const std::function<void()> callBack = [=]()
        {
            if (timerWPtr.expired())
                return;

            if (objectPtr.expired())
            {
                timerWPtr.lock()->Cancel();
                return;
            }


            if constexpr (std::is_invocable_v<Function, Timer*, Args...>)
	                function(timerWPtr.lock().get(), args...);
            else if constexpr (std::is_invocable_v<Function, Args...>)
                function(args...);
            else
            {
	            const auto* component{ dynamic_cast<Component*>(objectPtr.lock().get()) };
                Log(LogType::Error) << "Timer can't call function with given arguments! Timer will be canceled! Timer called on: " << (component ? component->GetName() : "Not a component");
                timerWPtr.lock()->Cancel();
            }
        };

        timerSPtr->m_CallBack = callBack;
        m_TimerPtrs.insert(timerSPtr);
        return timerSPtr;
    }


    template<typename Function, typename ObjectType>
    static void StartLerpTimer(std::weak_ptr<ObjectType> objectPtr, Function&& function)
    {
        StartLerpTimer(1.0f, false, objectPtr, function);
    }

    template<typename Function, typename ObjectType>
    static void StartLerpTimer(float time, std::weak_ptr<ObjectType> objectPtr, Function&& function)
    {
        StartLerpTimer(time ,false, objectPtr, function);
    }

    template<typename Function, typename ObjectType>
    static void StartLerpTimer(float time,bool ignoreTimeScale, std::weak_ptr<ObjectType> objectPtr, Function&& function)
    {
        // Force call with 0
        function(0.0f);

        const std::shared_ptr timerSPtr{ std::make_shared<Timer>(0.0f,true,ignoreTimeScale,nullptr) };
        const std::weak_ptr timerWPtr{ timerSPtr };

        const std::function<void()> callBack = [=]()
        {
            if (timerWPtr.expired())
                return;

            if (objectPtr.expired())
            {
                timerWPtr.lock()->Cancel();
                return;
            }

            if (timerWPtr.lock()->m_ElapsedTime > time)
            {
                timerWPtr.lock()->m_ElapsedTime = time;
                timerWPtr.lock()->Cancel();
            }

            function(timerWPtr.lock()->m_ElapsedTime / time);
        };

        timerSPtr->m_CallBack = callBack;
        m_TimerPtrs.insert(timerSPtr);
    }


    static void UpdateTimers();

    static void CleanTimers();


    static void SetDeltaTime(float deltaTime) { m_DeltaTime = deltaTime; }
    static void AddToFrameCount() { m_FrameCount++; }

    static void SetTimeScale(float timeScale);

    [[nodiscard]] static float GetDeltaTime() { return m_DeltaTime * m_TimeScale; }
    [[nodiscard]] static float GetUnScaledDeltaTime() { return m_DeltaTime; }
    [[nodiscard]] static int GetFrameCount() { return m_FrameCount; }
    [[nodiscard]] static float GetTimeScale() { return m_TimeScale; }

private:

    inline static float m_DeltaTime{ 0 };
    inline static int m_FrameCount{ 0 };
    inline static float m_TimeScale{1.0f};

    inline static std::unordered_set<std::shared_ptr<Timer>> m_TimerPtrs{};
};





/// All the following timer functions are temporally suspended, They don't take in to account the life time of the object
/* Timers without object ptr are disabled for now with safty concerns due to objects being destroyed why timer is running



    //// Function can be cleaned with better timer class specificity for this use case
    //template<typename Function, typename ObjectType, typename... Args>
    //static std::shared_ptr<Timer> SetUpdateTimer(std::weak_ptr<ObjectType> objectPtr, Function&& function, Args&&... args)
    //{
    //    return SetTimer(0, true, objectPtr, function, args...);
    //}




// Set timer on static/global function with optional arguments
template<typename Function, typename... Args>
static std::shared_ptr<Timer> SetTimer(float delay, bool looping, Function&& function, Args&&... args)
{
    auto newTimer{ std::make_shared<Timer>(delay,looping, std::bind(std::forward<Function>(function), std::forward<Args>(args)...)) };
    m_TimerPtrs.insert(newTimer);
    return newTimer;
}

// Sets a timer and returns the timer to the function
template<typename Function>
static std::shared_ptr<Timer> SetTimerInline(float delay, bool looping, Function&& function)
{
    auto newTimer{ std::make_shared<Timer>(delay,looping,nullptr) };
    auto callBack = std::bind(std::forward<Function>(function), newTimer.get());

    newTimer->m_CallBack = callBack;
    m_TimerPtrs.insert(newTimer);
    return newTimer;
}

 Update loop not in use atm

template<typename Function, typename ObjectType, typename... Args>
static void StartUpdateLoop(std::weak_ptr<ObjectType> objectPtr,Function&& function, Args&&... arg)
{
    StartUpdateLoop(0.0f, true, objectPtr, function, arg);
}


template<typename Function>
static void StartLerp(Function&& function)
{
    StartLerp(1.0f, function);
}

template<typename Function>
static void StartLerp(float time, Function&& function)
{
    // Force call with 0
    function(0.0f);

    const auto newTimer{ std::make_shared<Timer>(0.0f,true,nullptr) };

    const std::weak_ptr weakTimer{ newTimer };
    const std::function<void()> callBack = [=]()
    {
        if (weakTimer.lock()->m_ElapsedTime > time)
        {
            weakTimer.lock()->m_ElapsedTime = time;
            weakTimer.lock()->Cancel();
        }

        function(weakTimer.lock()->m_ElapsedTime / time);
    };

    newTimer->m_CallBack = callBack;
    m_TimerPtrs.insert(newTimer);
}


    template<typename Function, typename ObjectType>
    static void SetTimerOnObjectInline(float delay, bool looping, std::weak_ptr<ObjectType> objectPtr, Function&& function)
    {

        const auto newTimer{ std::make_shared<Timer>(0.0f,true,nullptr) };

        const std::weak_ptr weakTimer{ newTimer };
        const std::function<void()> callBack = [=]()
        {
            if (objectPtr.expired())
            {
                weakTimer.lock()->Cancel();
                return;
            }

            function(weakTimer.lock().get());
        };

        newTimer->m_CallBack = callBack;
        m_TimerPtrs.insert(newTimer);
        //return newTimer;
    }




        template<typename Function, typename ObjectType, typename... Args>
    static void SetTimer(float delay, bool looping, std::weak_ptr<ObjectType> objectPtr, Function&& function, Args&&... args)
    {
        const auto newTimer{ std::make_shared<Time::Timer>(delay, looping, nullptr) };
        const std::weak_ptr weakTimer{ newTimer };

        const auto callBack = [=]()
        {
            if (objectPtr.expired()) 
            {
                weakTimer.lock()->Cancel();
                return;
            }

            // Check if Function is a member function pointer
            if constexpr (std::is_member_function_pointer_v<std::decay_t<Function>>)
            {
                auto memberFunction = std::mem_fn(function);
                auto targetObject = objectPtr.lock();

                memberFunction(targetObject.get(), args...);


                //if constexpr (std::is_invocable_v<decltype(memberFunction), ObjectType*, Args...>)
                //else
                //    Log(LogType::Error) << "Timer can't call member function with given arguments!";
            }
            else 
            {
                if constexpr (std::is_invocable_v<Function, Timer*, Args...>)
                    function(weakTimer.lock().get(), args...);
                else if constexpr (std::is_invocable_v<Function, Args...>)
                    function(args...);
                else
                    Log(LogType::Error) << "Timer can't call function with given arguments!";
            }
        };

    	newTimer->m_CallBack = callBack;
		m_TimerPtrs.insert(newTimer);
    }

*/
