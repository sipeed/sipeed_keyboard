
import cv2
import numpy

def getkeypos():
    endpos=30
    pos1=[]
    pos2=[]
    pos3=[]
    pos4=[]
    pos5=[]

    xpos=endpos
    ypos=1.9*4

    leds=[];
    leds.insert(0,2.4)
    leds.insert(0,2.4)
    leds.insert(0,7.2)
    leds.insert(0,6.9)
    a=([1.9]*5)
    leds=a+leds

    pos1.append((xpos,ypos))

    for a in leds:
        xpos-=a
        pos1.insert(0,(xpos, ypos))

    xpos=endpos
    ypos=1.9*3

    leds=[];
    leds.insert(0,3.1)
    a=([1.9]*9)
    leds=a+leds
    leds.insert(0,2.6)
    leds.insert(0,2.6)
    leds.insert(0,1.9)


    pos2.append((xpos,ypos))

    for a in leds:
        xpos-=a
        pos2.append((xpos, ypos))



    xpos=endpos
    ypos=1.9*2

    leds=[];
    leds.insert(0,2.6)
    a=([1.9]*10)
    leds=a+leds
    leds.insert(0,3.1)
    leds.insert(0,3.1)

    pos3.append((xpos,ypos))

    for a in leds:
        xpos-=a
        pos3.insert(0,(xpos, ypos))


    xpos=endpos
    ypos=1.9*1

    leds=[];
    leds.insert(0,2.4)
    a=([1.9]*11)
    leds=a+leds
    leds.insert(0,2.4)
    leds.insert(0,2.4)


    pos4.append((xpos,ypos))

    for a in leds:
        xpos-=a
        pos4.append((xpos, ypos))


    xpos=endpos
    ypos=1.9*0

    leds=[];
    a=([1.9]*12)
    leds=a+leds
    leds.insert(0,2.85)
    leds.insert(0,2.85)

    pos5.append((xpos,ypos))

    for a in leds:
        xpos-=a
        pos5.insert(0,(xpos, ypos))

    # print("keys:",len(pos),pos)
    #
    # mat=numpy.zeros((400,400,3))
    # for l in pos:
    #     mat=cv2.circle(mat,(int(l[0]*10+50),int(l[1]*10+50)),5,(255,255,255))
    # cv2.imshow("keys",mat)
    # cv2.waitKey(0)
    pos=pos1+pos2+pos3+pos4+pos5
    pos=numpy.array(pos)
    return pos
    # print(pos)