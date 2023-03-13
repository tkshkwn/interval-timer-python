from distutils.core import setup, Extension

with open('README.md', 'r') as fp:
    readme = fp.read()

setup(
    name='interval-timer-python',
    version='0.1.1',
    license='MIT-License',
    description='interval-timer for python: Windows:waitabletimer wrapper, Linux:timerfd wrapper',
    long_description=long_description,
    long_description='text/markdown',
    author='tkshkwn',
    author_email='nekotk37@gmail.com',
    url='https://github.com/tkshkwn/interval-timer-python',
    keywords="timer, periodic, interval, timerfd",
    ext_modules = [Extension('interval_timer', ['interval_timer_python.c', 'interval_timer.c'])]
)
