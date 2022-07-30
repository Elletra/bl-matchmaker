// ----------------------------------------------------------------------------
// Code from: Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
// ----------------------------------------------------------------------------

#ifndef _BITSTREAM_BITSTREAM_H
#define _BITSTREAM_BITSTREAM_H

#include <exception>

#include "util.h"
#include "types.h"



// This should ideally be done with templates...
//
/// @defgroup stream_overload Primitive Type Stream Operation Overloads
/// These macros define the read and write functions for all primitive types.
/// @{

#define DECLARE_OVERLOADED_READ(type)      \
   bool read(type* out_read) {             \
      return read(sizeof(type), out_read); \
   }
#define DECLARE_OVERLOADED_WRITE(type)       \
   bool write(type in_write) {               \
      return write(sizeof(type), &in_write); \
   }

#define DECLARE_ENDIAN_OVERLOADED_READ(type)       \
   bool read(type* out_read) {                     \
      type temp;                                   \
      bool success = read(sizeof(type), &temp);    \
      *out_read = temp;/*convertLEndianToHost(temp);*/      \
      return success;                              \
   }
#define DECLARE_ENDIAN_OVERLOADED_WRITE(type)      \
   bool write(type in_write) {                     \
      type temp = in_write;/*convertHostToLEndian(in_write);*/  \
      return write(sizeof(type), &temp);           \
   }
/// @}

class BitStream
{
public:

protected:
	U8 *dataPtr;
	S32  bitNum;
	S32  bufSize;
	bool error;
	S32  maxReadBitNum;
	S32  maxWriteBitNum;
	char *stringBuffer;

	friend class HuffmanProcessor;
public:
	enum Status {
		Ok = 0,           ///< Ok!
		IOError,          ///< Read or Write error
		EOS,              ///< End of Stream reached (mostly for reads)
		IllegalCall,      ///< An unsupported operation used.  Always w/ accompanied by AssertWarn
		Closed,           ///< Tried to operate on a closed stream (or detached filter)
		UnknownError      ///< Catchall
	};
	// Accessible only through inline accessors
private:
	Status m_streamStatus;

public:
	void setStatus (const Status in_newStatus) { m_streamStatus = in_newStatus; }

	// Overloaded write and read ops..
public:
	bool read (const U32 in_numBytes, void *out_pBuffer) {
		return _read (in_numBytes, out_pBuffer);
	}
	bool write (const U32 in_numBytes, const void *in_pBuffer) {
		return _write (in_numBytes, in_pBuffer);
	}
	DECLARE_OVERLOADED_WRITE (S8)
	DECLARE_OVERLOADED_WRITE (U8)

	DECLARE_ENDIAN_OVERLOADED_WRITE (S16)
	DECLARE_ENDIAN_OVERLOADED_WRITE (S32)
	DECLARE_ENDIAN_OVERLOADED_WRITE (U16)
	DECLARE_ENDIAN_OVERLOADED_WRITE (U32)
	DECLARE_ENDIAN_OVERLOADED_WRITE (F32)
	DECLARE_ENDIAN_OVERLOADED_WRITE (F64)

	DECLARE_OVERLOADED_READ (S8)
	DECLARE_OVERLOADED_READ (U8)

	DECLARE_ENDIAN_OVERLOADED_READ (S16)
	DECLARE_ENDIAN_OVERLOADED_READ (S32)
	DECLARE_ENDIAN_OVERLOADED_READ (U16)
	DECLARE_ENDIAN_OVERLOADED_READ (U32)
	DECLARE_ENDIAN_OVERLOADED_READ (F32)
	DECLARE_ENDIAN_OVERLOADED_READ (F64)

	void setBuffer (void *bufPtr, S32 bufSize, S32 maxSize = 0);
	U8 *getBuffer () { return dataPtr; }
	U8 *getBytePtr ();

	U32 getReadByteSize ();

	S32  getCurPos () const;
	void setCurPos (const U32);

	BitStream (void *bufPtr, S32 bufSize, S32 maxWriteSize = -1) { setBuffer (bufPtr, bufSize, maxWriteSize); stringBuffer = NULL; }
	void clear ();

	void writeInt (S32 value, S32 bitCount);
	S32  readInt (S32 bitCount);

	void writeSignedInt (S32 value, S32 bitCount);
	S32  readSignedInt (S32 bitCount);

	// read and write floats... floats are 0 to 1 inclusive, signed floats are -1 to 1 inclusive

	F32  readFloat (S32 bitCount);
	F32  readSignedFloat (S32 bitCount);

	void writeFloat (F32 f, S32 bitCount);
	void writeSignedFloat (F32 f, S32 bitCount);

	virtual void writeBits (S32 bitCount, const void *bitPtr);
	virtual void readBits (S32 bitCount, void *bitPtr);
	virtual bool writeFlag (bool val);
	virtual bool readFlag ();

	void setBit (S32 bitCount, bool set);
	bool testBit (S32 bitCount);

	bool isFull () { return bitNum > (bufSize << 3); }
	bool isValid () { return !error; }

	bool _read (const U32 size, void *d);
	bool _write (const U32 size, const void *d);

	void readString (char stringBuf[256]);
	void writeString (const char *stringBuf, S32 maxLen = 255);

	U32  getPosition () const;
	bool setPosition (const U32 in_newPosition);
	U32  getStreamSize ();
};

//------------------------------------------------------------------------------
//-------------------------------------- INLINES
//
inline S32 BitStream::getCurPos () const
{
	return bitNum;
}

inline void BitStream::setCurPos (const U32 in_position)
{
	AssertFatal (in_position < (U32) (bufSize << 3), "Out of range bitposition");
	bitNum = S32 (in_position);
}

inline bool BitStream::readFlag ()
{
	if (bitNum > maxReadBitNum)
	{
		error = true;
		AssertFatal (false, "Out of range read");
		return false;
	}
	S32 mask = 1 << (bitNum & 0x7);
	bool ret = (*(dataPtr + (bitNum >> 3)) & mask) != 0;
	bitNum++;
	return ret;
}


#endif
