//------------------------------------------------------------------------------
/**
    Simple static C coroutine macros inspired by Simon Tatham see:
    http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html

    The coroutine requires the following signature:

    void Coroutine(void);

    It can be used as follows:

    void Coroutine(void)
    {
        static int i;
        CE_STATIC_COROUTINE_BEGIN
        for (i = 0; i < 10; ++i)
        {
            // the code to execute by the coroutine
            CE_STATIC_COROUTINE_YIELD
        }
        CE_STATIC_COROUTINE_END
    }

    (C) 2016 Christian Bleicher
*/
#define CE_STATIC_COROUTINE_BEGIN static unsigned char state = 0; switch (state) { case 0:
#define CE_STATIC_COROUTINE_YIELD state = 1; return; case 1:;
#define CE_STATIC_COROUTINE_END }
