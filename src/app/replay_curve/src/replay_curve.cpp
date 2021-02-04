#include "replay_curve.hpp"
#include "trace_if.h"

namespace app
{
    void ReplayCurve::load_data(const app::CharacteristicCurve<int32_t, int32_t> &o_data)
    {
        m_o_data = o_data;
    }

    void ReplayCurve::pause()
    {
        if (true == bo_playing)
        {
            bo_paused = true;
        }

    }
    void ReplayCurve::play()
    {
        if (true == bo_playing && true == bo_paused)
        {
            m_o_last_time_point = std::chrono::system_clock::now();
            bo_paused = false;
        }
        else if (false == bo_playing)
        {
            // take the starting timestamp, and reset the current duration to 0
            m_o_last_time_point = std::chrono::system_clock::now();
            m_o_current_duration = std::chrono::milliseconds(0);
            bo_playing = true;

        }
    }

    void ReplayCurve::stop()
    {
        if (true == bo_playing)
        {
            bo_playing = false;
            bo_paused = false;
        }
    }

    void ReplayCurve::cycle()
    {
        if (true == bo_playing)
        {
            if (false == bo_paused)
            {
                // measure how much time has passed, and add it to the duration
                auto current_time_point = std::chrono::system_clock::now();
                m_o_current_duration += std::chrono::duration_cast<std::chrono::milliseconds>(current_time_point - m_o_last_time_point);
                m_o_last_time_point = current_time_point;

                if (m_o_current_duration > std::chrono::milliseconds(this->m_o_data.get_last_x()))
                {
                    m_o_current_duration = std::chrono::milliseconds(0);
                }
            }

        }

    }


    std::chrono::milliseconds ReplayCurve::get_current_time() const
    {
        return m_o_current_duration;
    }

    int32_t ReplayCurve::get_current_data() const
    {
        return m_o_data.get_y(m_o_current_duration.count());

    }


}
