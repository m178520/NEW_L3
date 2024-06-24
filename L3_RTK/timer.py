import osTimer


class timer(object):
    def __init__(self,time = 1000) -> None:
        self.timer = osTimer()
        self.time = time

    def start(self,cyclialEn,fun):
        self.timer.start(self.time,cyclialEn,fun)
    
    def stop(self):
        self.timer.stop()