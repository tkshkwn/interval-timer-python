from distutils.core import setup, Extension

setup(
    name='interval-timer-python',
    version='0.1.0',
    license='MIT-License',
    description='interval-timer for python: Windows:waitabletimer wrapper, Linux:timerfd wrapper',

    author='tkshkwn',
    author_email='nekotk37@gmail.com',
    url='https://github.com/tkshkwn/interval-timer-python',

    keywords="timer, periodic, interval, timerfd",

    ext_modules = [Extension('interval_timer', ['interval_timer_python.c', 'interval_timer.c'])]
)
