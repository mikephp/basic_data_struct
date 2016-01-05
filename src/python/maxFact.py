#!/usr/bin/env python

def showMaxFactor(num):
    count = num / 2
    while count > 1:
        if (num % count == 0):
            print 'largest factor of %d is %d' % \
            (num, count)
            break
        elif count == 2:
            print eachNum, 'is prime'
        count = count - 1

for eachNum in range(10, 21):
    showMaxFactor(eachNum)
