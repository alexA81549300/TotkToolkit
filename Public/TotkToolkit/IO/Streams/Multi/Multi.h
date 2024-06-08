#pragma once

#include <Formats/IO/Stream.h>
#include <vector>

#define TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(call) for (std::shared_ptr<Formats::IO::Stream> stream : mStreams) {stream->call;}
#define TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(call, defaultReturn) for (int i = 1; i < mStreams.size(); i++) {mStreams[i]->call;} if (mStreams.size() > 0) { return mStreams[0]->call; } return defaultReturn;
#define TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_FIRST_RET(call, defaultReturn) if (mStreams.size() > 0) { return mStreams[0]->call; } return defaultReturn;

#define TOTKTOOLKIT_IO_STREAMS_MULTI_READ(type, name) virtual type Read##name() override {\
		TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(Read##name(), 0)\
	}
#define TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(type, name) virtual void Write##name(type value) override {\
		TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(Write##name(value))\
	}

namespace TotkToolkit::IO::Streams::Multi {
	/// <summary>
	/// Super specialty.
	/// Bundles a set of streams into one stream.
	/// All streams are used at once, and it is not guarenteed which
	/// one is returned for.
	/// </summary>
	class Multi : public Formats::IO::Stream {
	public:
		Multi(std::vector<std::shared_ptr<Formats::IO::Stream>> streams) : mStreams(streams) {

		}
		~Multi() {}

		virtual void Seek(std::streampos pos) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(Seek(pos)) }
		virtual void PushSeek(std::streampos pos) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(PushSeek(pos)) }
		virtual std::streampos PopSeek() override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(PopSeek(), -1) }
		virtual std::streampos GetSeek() override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_FIRST_RET(GetSeek(), -1) }
		virtual void AlignSeek(std::streampos alignment) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(AlignSeek(alignment)) }

		virtual bool Flush() override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(Flush(), true) }

		virtual std::string ReadZeroTerminatedString(F_U32 allocation = FORMATS_IO_STREAM_READ_STRING_DEFAULTALLOCATION) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(ReadZeroTerminatedString(allocation), "\0") }
		virtual void WriteZeroTerminatedString(std::string value) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(WriteZeroTerminatedString(value)) }

		virtual void SetEndianness(Formats::IO::Endianness endianness) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(SetEndianness(endianness)) }

		virtual F_U8 ReadU8() override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(ReadU8(), 0) }
		virtual F_S8 ReadS8() override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET(ReadS8(), 0) }
		virtual void WriteU8(F_U8 value) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(WriteU8(value)) }
		virtual void WriteS8(F_S8 value) override { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(WriteS8(value)) }

		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_U16, U16)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_U24, U24)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_U32, U32)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_U64, U64)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_S16, S16)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_S24, S24)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_S32, S32)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_S64, S64)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_F32, F32)
		TOTKTOOLKIT_IO_STREAMS_MULTI_READ(F_F64, F64)

		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_U16, U16)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_U24, U24)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_U32, U32)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_U64, U64)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_S16, S16)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_S24, S24)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_S32, S32)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_S64, S64)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_F32, F32)
		TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE(F_F64, F64)

		virtual void ReadBytes(void* out, F_U32 size) { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(ReadBytes(out, size)) }
		virtual void WriteBytes(const void* in, F_U32 size) { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL(WriteBytes(in, size)) }

		virtual std::shared_ptr<F_U8[]> GetBuffer() { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_FIRST_RET(GetBuffer(), nullptr) }
		virtual F_UT GetBufferLength() { TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_FIRST_RET(GetBufferLength(), 0) }

	protected:
		std::vector<std::shared_ptr<Formats::IO::Stream>> mStreams;
	};
}

#undef TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL
#undef TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_ALL_RET
#undef TOTKTOOLKIT_IO_STREAMS_MULTI_CALL_FIRST_RET
#undef TOTKTOOLKIT_IO_STREAMS_MULTI_READ
#undef TOTKTOOLKIT_IO_STREAMS_MULTI_WRITE