

class Chat:
    def __init__(self, sendMessage):
        self.sendMessage = sendMessage

    def send_test(self, string):
        d = {
            'msg' : str(string)
        }

        self.sendMessage.send_chat(d)
        print "Chat Message Sent"
