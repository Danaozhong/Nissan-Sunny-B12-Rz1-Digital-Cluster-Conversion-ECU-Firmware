#ifndef _REPLAY_CURVE_HPP_
#define _REPLAY_CURVE_HPP_

#include <chrono>
#include "lookup_table.hpp"

namespace app
{
    class ReplayCurve
    {
    public:
        //ReplayCurve() {}
        //ReplayCurve(const app::CharacteristicCurve<std::chrono::milliseconds, uint32_t> &o_data);

        void load_data(const app::CharacteristicCurve<int32_t, int32_t> &o_data);
        void pause();
        void play();
        void stop();

        void cycle();

        std::chrono::milliseconds get_current_time() const;
        int32_t get_current_data() const;
    private:
        std::chrono::milliseconds m_o_current_duration;
        std::chrono::system_clock::time_point m_o_last_time_point;
        app::CharacteristicCurve<int32_t, int32_t> m_o_data;

        bool bo_playing;
        bool bo_paused;
    };
}


#endif /* _REPLAY_CURVE_HPP_ */
