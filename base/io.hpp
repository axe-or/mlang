#include "base.hpp"

enum StreamMode : u8 {
	Stream_Query = 0,
	Stream_Read,
	Stream_Write,
	Stream_Seek,
	Stream_Close
};

enum StreamSeek : u8 {
	Seek_Start = 0,
	Seek_Current = 1,
	Seek_End = 2,
};

using StreamFunc = i64 (*)(
	void* impl,
	StreamMode mode,
	Slice<u8> buf,
	i64 offset,
	StreamSeek seek_from
);

struct Stream {
	StreamFunc _fn;
	void* _data;

	attribute_force_inline
	i64 read(Slice<u8> buf, i64 offset, StreamSeek seek = Seek_Current){
		return _fn(_data, Stream_Read, buf, offset, seek);
	}

	attribute_force_inline
	i64 write(Slice<u8> buf, i64 offset, StreamSeek seek = Seek_Current){
		return _fn(_data, Stream_Write, buf, offset, seek);
	}

	attribute_force_inline
	u8 query(){
		return (u8)_fn(_data, Stream_Query, {}, 0, (StreamSeek)0);
	}
};
