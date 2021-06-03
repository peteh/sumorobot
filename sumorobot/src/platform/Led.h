namespace sumobot
{
    class Led
    {
        public: 
        Led(unsigned int io, bool reversed = false);
        void init();
        void set(bool on);
        void on();
        void off();

        private: 
        unsigned int m_io;
        bool m_isOn;
        bool m_reversed;
    };
}