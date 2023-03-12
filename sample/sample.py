import time
from multiprocessing import Process

import interval_timer


def main1():
    itimer1 = interval_timer.IntervalTimer()
    res = itimer1.settime(100)
    if res is False:
        return
    try:
        while True:
            print(time.perf_counter(), "Hello")
            itimer1.wait()
    except KeyboardInterrupt:
        return

def main2():
    itimer2 = interval_timer.IntervalTimer()
    res = itimer2.settime(200)
    if res is False:
        return
    try:
        while True:
            print(time.perf_counter(), "World")
            itimer2.wait()
    except KeyboardInterrupt:
        return


if __name__ == '__main__':
    p1 = Process(target=main1)
    p1.start()
    p2 = Process(target=main2)
    p2.start()
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        p1.terminate()
        p2.terminate()

