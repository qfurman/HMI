
#ifndef _IS_INCLUDED_MAIN_H
#define _IS_INCLUDED_MAIN_H

extern unsigned int time_to_arch;//��� �� ���������� ����������� � �������
//extern A_MNG a_mng;//��������� �������� ���������

extern int arch_flash_addr;
extern int arch_page;//������� 0-16383 ������ � ���� ������� 256 ���� - 16 �����
extern int next_arch_page;//������� 0-16383 ������ � ���� ������� 256 ���� - 16 ������   ��� ���������� ��������

extern char ERROR;//��� ������� ���� �� �������
extern volatile int ERROR_TIME;//��� �� ����� ����� �������
extern char ERROR_I;//�Ͳֲ���� ������� ���������� � �Ѳ ��ز

#endif
