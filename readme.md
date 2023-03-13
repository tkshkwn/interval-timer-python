## interval-timer-python

### overview
- interval timer library for python
    - on Windows: Waitable Timer wrapper
    - on Linux: timerfd wrapper

### OS
- Windows & Linux

### installation
```bash
python setup.py install
```

### usage
```python
import interval_timer

INTERVAL_TIME_MS = 100
itimer = interval_timer.IntervalTimer()
itimer.settime(INTERVAL_TIME_MS)
while True:
    print("Hello,World!")
    itimer.wait()  # periodic blocking
```
