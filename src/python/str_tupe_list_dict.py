#-*-coding:utf-8-*-

#1���ֵ�
dict = {'name': 'Zara', 'age': 7, 'class': 'First'}
print "dict" ,dict
#�ֵ�תΪ�ַ��������أ�<type 'str'> {'age': 7, 'name': 'Zara', 'class': 'First'}
print type(str(dict)), str(dict)

#�ֵ����תΪԪ�飬���أ�('age', 'name', 'class')
print tuple(dict.keys())
#�ֵ����תΪԪ�飬���أ�(7, 'Zara', 'First')
print tuple(dict.values())

#�ֵ�תΪ�б����أ�['age', 'name', 'class']
print list(dict)
#�ֵ�תΪ�б�
print dict.values

#2��Ԫ��
tup=(1, 2, 3, 4, 5)

#Ԫ��תΪ�ַ��������أ�(1, 2, 3, 4, 5)
print tup.__str__()

#Ԫ��תΪ�б����أ�[1, 2, 3, 4, 5]
print list(tup)

#Ԫ�鲻����תΪ�ֵ�

#3���б�
nums=[1, 3, 5, 7, 8, 13, 20];

#�б�תΪ�ַ��������أ�[1, 3, 5, 7, 8, 13, 20]
print str(nums)

#�б�תΪԪ�飬���أ�(1, 3, 5, 7, 8, 13, 20)
print tuple(nums)

#�б�����תΪ�ֵ�

#4���ַ���

#�ַ���תΪԪ�飬���أ�(1, 2, 3)
print tuple(eval("(1,2,3)"))
#�ַ���תΪ�б����أ�[1, 2, 3]
print list(eval("(1,2,3)"))
#�ַ���תΪ�ֵ䣬���أ�<type 'dict'>
print type(eval("{'name':'ljq', 'age':24}"))