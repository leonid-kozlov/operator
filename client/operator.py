import argparse

import requests

class Operator:
    url = None

    def __init__(self, url):
        self.url = url

    def led_on(self):
        requests.post(f"{self.url}/operator/led_on", verify=False)

    def led_off(self):
        requests.post(f"{self.url}/operator/led_off", verify=False)

    def play_note(self, note, duration):
        requests.post(f"{self.url}/operator/play/{note}/{duration}", verify=False)

    def vtykatel(self, action):
        assert action in ["in", "out"]
        requests.post(f"{self.url}/operator/vtykatel_{action}", verify=False)


def main():
    parser = argparse.ArgumentParser(description="Operator helper")
    parser.add_argument("command", choices=["led", "play", "vtykatel"], help="What to do")
    parser.add_argument("data", type=str, help="operand")
    args = parser.parse_args()

    operator = Operator("http://192.168.3.3")
    if args.command == "led":
        if args.data == "on":
            operator.led_on()
        if args.data == "off":
            operator.led_off()
    if args.command == "play":
        operator.play_note(args.data, "1000")
    if args.command == "vtykatel":
        operator.vtykatel(args.data)


if __name__ == "__main__":
    main()
