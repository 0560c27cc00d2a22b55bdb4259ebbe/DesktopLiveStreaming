#pragma once

#include "AppContext.h"
#include "FLVhelp.h"

#define DefaultItemSize (4*1024)
#define rcb_lock_value (4)

enum TagType {
	VideoTag,
	AudioTag,
	ScriptTag,
	OtherTag
};

typedef struct _RCBItem {

	TagType tagType;
	bool isKeyFrame;

	char tag_body_default[DefaultItemSize];//4kbĬ��
	char *tag_body_big;//
	bool tag_body_isbig;
	size_t tag_body_length;

	char flv_tag_header[20];// size 11+9�̶���С 11��tagͷ 9����Ƶͷ ��ƵҲ������ֶ�

	char *flv_tag_real;//��Ҫ��h264��ƫ���������������ݵ�ַ������������h264nal�� 00 00 00 01
	size_t flv_tag_real_length;//����֮������ݳ���

	QWORD compositionTime;
	QWORD  timems;
	_RCBItem* next;
	volatile int index;
}RCBItem;

typedef struct FlvTagHeader
{
	char data[12];// size 11�̶���С
	WSABUF wsabuf;
};


class RingCacheBuffer
{

public:
	//
	RingCacheBuffer(int ringLength);
	~RingCacheBuffer();

	void overlay_video(bool isKeyFrame, char *data, int length, DWORD timems, int compositionTime);
	void overlay_audio(char *data, int length, DWORD timems);

	friend class FlvLiveStream;
	friend class HLS_Server;

private:
	HANDLE new_event;

	int RingLength;
	volatile RCBItem* LastWrite;//����ʵ��ѡ��д��

	volatile RCBItem* ReadLastWrite;//���ڶ�ȡ�ϴ�д��

	RCBItem* items;
	RCBItem* lastKeyFrame;//�ϸ��ؼ�֡

	CRITICAL_SECTION overlay_cs;
	app_atomic_lock_t lock_t;

	volatile unsigned int tag_index;
};