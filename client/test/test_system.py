import os
import time
import sys

__location__ = os.path.realpath(os.path.join(os.getcwd(), os.path.dirname(__file__)))
__location_maildir__ = __location__ + "/../maildir/"

def test_case_1():
    pass

def test_case_2():
    pass

def run():
    print __location__
    old_file = __location_maildir__ + "testemail1.tmp"
    new_file = __location_maildir__ + "testemail1.eml"
    file = open(old_file, "w+")
    file.write("from: vasya@gmail.com\n"
    "to: vasya@localhost\n"
    "subject: Smtp Client Test\n"
    "Here Is SMTP CLIENT TEST DATA TEXT\n"
    )
    os.rename(old_file, new_file)
    expected_string = "From: <vasya@gmail.com>\n" + \
                      "To: <vasya@localhost>\n" + \
                      "Subject: Smtp Client Test\n"+ \
                      "MIME-Version:1.0\n" + \
                      "Content-Type: text/plain; charset=\"utf-8\"\n" + \
                      "\n" + \
                      "Here Is SMTP CLIENT TEST DATA TEXT\n"+ \
                      "\n"

    expected_file_name = __location__ + "/vasya@gmail.com.eml"
    count = 0
    while True:
        if count > 5:
            sys.stderr.write("File does not exists. Mail has been sent with errors.")
            return
        if os.path.isfile(expected_file_name):
            break
        count += 1
        time.sleep(1)
    expected_file = open(expected_file_name, "r")
    file_string = expected_file.read()

    if expected_string != file_string:
        sys.stderr.write("Test failed. Expected mail content:\n%s\nbut found:\n%s" % (expected_string, file_string))
        return

    print "Test passed"

if __name__ == '__main__':
    run()