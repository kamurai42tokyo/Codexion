[目的]
複数の処理（コーダー、モニター）を同時に動かす。

[準備]
・#include <pthread.h>
・コンパイル時に -pthread

[スレッドで動かす関数]
    void	*coder_routine(void *arg);
・引数  ：arg  pthread_create の4つ目で渡した値
・返り値：void *  pthread_join の2つ目で受け取れる（不要なら NULL を返す）

[pthread_create]  スレッドを作って動かし始める
    int	pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    		void *(*start_routine)(void *), void *arg);
・thread       ：作ったスレッドの値を書き込む場所
・attr         ：設定。普通は NULL
・start_routine：動かす関数
・arg          ：その関数に渡す引数
・返り値       ：成功 0、失敗 0以外

[pthread_join]  スレッドが終わるまで待つ
    int	pthread_join(pthread_t thread, void **retval);
・thread：待つスレッド
・retval：スレッドの返り値を受け取る場所。不要なら NULL
・返り値：成功 0、失敗 0以外

[pthread_mutex_init]  鍵を準備する（最初に1回）
    int	pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
・mutex ：準備する鍵
・attr  ：設定。普通は NULL
・返り値：成功 0、失敗 0以外

[pthread_mutex_lock]  鍵を取る（使用中なら待つ）
    int	pthread_mutex_lock(pthread_mutex_t *mutex);
・mutex ：取る鍵
・返り値：成功 0、失敗 0以外

[pthread_mutex_unlock]  鍵を返す
    int	pthread_mutex_unlock(pthread_mutex_t *mutex);
・mutex ：返す鍵
・返り値：成功 0、失敗 0以外

[pthread_mutex_destroy]  鍵を片付ける（最後に1回）
    int	pthread_mutex_destroy(pthread_mutex_t *mutex);
・mutex ：片付ける鍵
・返り値：成功 0、失敗 0以外
