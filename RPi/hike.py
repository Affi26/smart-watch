from datetime import datetime, timezone

MET_HIKING = 6
KCAL_PER_STEP = 0.04

class HikeSession:
    id = 0
    distance = 0
    steps = 0
    kcal = -1
    session_time = datetime.now().strftime("%y%m%d%H%M") #returns string
    duration = 0
    coords = []

    # represents a computationally intensive calculation done by lazy execution.
    def calc_kcal(self):
        self.kcal = MET_HIKING * KCAL_PER_STEP * self.steps

    def __repr__(self):
        #return f"HikeSession{{{self.id}, {self.distance}(m), {self.steps}(steps), {self.kcal:.2f}(kcal)}}"
        return f"HikeSession{{{self.id}, {self.distance}(m), {self.steps}(steps), {self.kcal:.2f}(kcal), {self.duration}(sec), {self.session_time}(session time)}}"
        #return f"HikeSession{{{self.id}, {self.distance}(m), {self.steps}(steps), {self.kcal:.2f}(kcal), {self.session_time}(session time)}}"

def to_list(s: HikeSession) -> list:
    #return [s.id, s.distance, s.steps, s.kcal]
    return [s.id, s.distance, s.steps, s.kcal, s.duration, s.session_time]
    #return [s.id, s.distance, s.steps, s.kcal, s.session_time]


def from_list(l: list) -> HikeSession:
    s = HikeSession()
    s.id = l[0]
    s.distance = l[1]
    s.steps = l[2]
    s.kcal = l[3]
    s.duration = l[4]
    s.session_time = l[5]
    return s
