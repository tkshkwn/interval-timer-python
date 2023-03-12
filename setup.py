from distutils.core import setup, Extension

setup(
    name='interval_timer',
    version='0.1.0',
    license='proprietary',
    description='Module Experiment',

    author='tkshkwn',
    author_email='nekotk37@gmail.com',
    url='None.com',

    ext_modules = [Extension('interval_timer', ['interval_timer_python.c', 'interval_timer.c'])]
)
