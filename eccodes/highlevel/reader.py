import eccodes
import gribapi
from gribapi import ffi

from .message import GRIBMessage


class ReaderBase:
    def __init__(self):
        self._peeked = None

    def __iter__(self):
        return self

    def __next__(self):
        if self._peeked is not None:
            msg = self._peeked
            self._peeked = None
            return msg
        handle = self._next_handle()
        if handle is None:
            raise StopIteration
        return GRIBMessage(handle)

    def _next_handle(self):
        raise NotImplementedError

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        pass

    def peek(self):
        """Return the next available message without consuming it"""
        if self._peeked is None:
            handle = self._next_handle()
            if handle is not None:
                self._peeked = GRIBMessage(handle)
        return self._peeked


class FileReader(ReaderBase):
    """Read messages from a file"""

    def __init__(self, path):
        super().__init__()
        self.file = open(path, "rb")

    def _next_handle(self):
        return eccodes.codes_new_from_file(self.file, eccodes.CODES_PRODUCT_GRIB)

    def __enter__(self):
        self.file.__enter__()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        return self.file.__exit__(exc_type, exc_value, traceback)


class MemoryReader(ReaderBase):
    """Read messages from memory"""

    def __init__(self, buf):
        super().__init__()
        self.buf = buf

    def _next_handle(self):
        if self.buf is None:
            return None
        handle = eccodes.codes_new_from_message(self.buf)
        self.buf = None
        return handle


@ffi.callback("long(*)(void*, void*, long)")
def pyread_callback(payload, buf, length):
    stream = ffi.from_handle(payload)
    read = stream.read(length)
    n = len(read)
    ffi.buffer(buf, length)[:n] = read
    return n if n > 0 else -1  # -1 means EOF


try:
    cstd = ffi.dlopen(None)  # Raises OSError on Windows
    ffi.cdef("void free(void* pointer);")
    ffi.cdef(
        "void* wmo_read_any_from_stream_malloc(void*, long (*stream_proc)(void*, void*, long), size_t*, int*);"
    )
except OSError:
    cstd = None


def codes_new_from_stream(stream):
    if cstd is None:
        raise OSError("This feature is not supported on Windows")
    sh = ffi.new_handle(stream)
    length = ffi.new("size_t*")
    err = ffi.new("int*")
    err, buf = gribapi.err_last(gribapi.lib.wmo_read_any_from_stream_malloc)(
        sh, pyread_callback, length
    )
    buf = ffi.gc(buf, cstd.free, size=length[0])
    if err:
        if err != gribapi.lib.GRIB_END_OF_FILE:
            gribapi.GRIB_CHECK(err)
        return None

    # TODO: remove the extra copy?
    handle = gribapi.lib.grib_handle_new_from_message_copy(ffi.NULL, buf, length[0])
    if handle == ffi.NULL:
        return None
    else:
        return gribapi.put_handle(handle)


class StreamReader(ReaderBase):
    """Read messages from a stream (an object with a ``read`` method)"""

    def __init__(self, stream):
        if cstd is None:
            raise OSError("This feature is not supported on Windows")
        super().__init__()
        self.stream = stream

    def _next_handle(self):
        return codes_new_from_stream(self.stream)
