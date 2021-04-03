#include <string.h>

#include "table_object.h"
#include "bits.h"

#define METADATA_SIZE     (sizeof(_state)+sizeof(_errorCode)+sizeof(_size)+sizeof(_crc))

beforeTableUnloadCallback TableObject::_beforeUnload = 0;

void TableObject::addBeforeTableUnloadCallback(beforeTableUnloadCallback func)
{
    _beforeUnload = func;
}

beforeTableUnloadCallback TableObject::getBeforeTableUnloadCallback()
{
    return _beforeUnload;
}

TableObject::TableObject(Platform& platform)
    : _platform(platform)
{

}

void TableObject::readProperty(PropertyID id, uint32_t start, uint32_t& count, uint8_t* data)
{
    switch (id)
    {
        case PID_LOAD_STATE_CONTROL:
            data[0] = _state;
            break;
        case PID_TABLE_REFERENCE:
            if (_state == LS_UNLOADED)
                pushInt(0, data);
            else
                pushInt(tableReference(), data);
            break;
        case PID_MCB_TABLE:
            data = pushInt(_size, data); // segment size
            data = pushByte(0x00, data);  // CRC Control Byte
            data = pushByte(0xFF, data);  // ReadAccess/WrtiteAccess
            // pushWord(Crc16Citt(_data, _size), data);
            // print("Send CRC to ETS: ");
            // println(_crc);
            pushWord(_crc, data);
            // pushWord(tableReference() & 0xFFFF, data);
            break;
        case PID_ERROR_CODE:
            data[0] = _errorCode;
            break;
        default:
            InterfaceObject::readProperty(id, start, count, data);
    }
}

uint16_t TableObject::Crc16Citt(uint8_t* data, uint16_t length)
{
    uint16_t initialValue = 0x1D0F;
    uint16_t Table[] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, 0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

    if (data == nullptr)
        return 0;
    if (length == 0)
        return 0;

    uint16_t crc = initialValue;
    for (int i = 0; i <= length; i++)
    {
        crc = (uint16_t)((crc << 8) ^ Table[((crc >> 8) ^ (0xff & data[i]))]);
    }
    return crc;
}

void TableObject::writeProperty(PropertyID id, uint8_t start, uint8_t* data, uint8_t count)
{
    switch (id)
    {
        case PID_LOAD_STATE_CONTROL:
            loadEvent(data);
            break;

        //case PID_MCB_TABLE:
        //    TODO
        //    break;
        default:
            InterfaceObject::writeProperty(id, start, data, count);
    }
}

uint8_t TableObject::propertySize(PropertyID id)
{
    switch (id)
    {
        case PID_LOAD_STATE_CONTROL:
            return 1;
        case PID_TABLE_REFERENCE:
            return 4;
        case PID_ERROR_CODE:
            return 1;
        case PID_OBJECT_TYPE:
            return 2;
        case PID_MCB_TABLE:
            return 8;
        default:
            return InterfaceObject::propertySize(id);
    }
}

TableObject::~TableObject()
{
    if (_data != 0)
        _platform.freeMemory(_data);
}

LoadState TableObject::loadState()
{
    return _state;
}

void TableObject::beforeStateChange(LoadState& newState)
{
    if (newState == LS_UNLOADED) {
        if (_beforeUnload != 0)
            _beforeUnload(*this, newState);
    }
}

void TableObject::loadState(LoadState newState)
{
    if (newState == _state)
        return;
    beforeStateChange(newState);
    _state = newState;
}

void TableObject::save()
{
    if(_data == NULL)
        return;

    uint8_t* addr =_data - METADATA_SIZE;

    _platform.pushNVMemoryByte(_state, &addr);
    _platform.pushNVMemoryByte(_errorCode, &addr);
    _platform.pushNVMemoryInt(_size, &addr);
    _platform.pushNVMemoryWord(_crc, &addr);
}

void TableObject::restore(uint8_t* startAddr)
{
    uint8_t* addr = startAddr;
    _state = (LoadState)_platform.popNVMemoryByte(&addr);
    _errorCode = (ErrorCode)_platform.popNVMemoryByte(&addr);
    _size = _platform.popNVMemoryInt(&addr);
    _crc = _platform.popNVMemoryWord(&addr);

    if (_size > 0)
        _data = addr;
    else {
        _data = 0;
        _crc = 0;
    }
}

uint32_t TableObject::tableReference()
{
    return (uint32_t)(_data - _platform.referenceNVMemory());
}

bool TableObject::allocTable(uint32_t size, bool doFill, uint8_t fillByte)
{
    if (_data)
    {
        _platform.freeNVMemory(_ID);
        _data = 0;
        _size = 0;
        _crc = 0;
    }

    if (size == 0)
        return true;
    
    _data = _platform.allocNVMemory(size+this->size(), _ID);
    _data = _data + this->size();  //skip metadata
    _size = size;
    if (doFill){
        uint8_t* addr = _data;
        for(size_t i=0; i<_size;i++)
            _platform.writeNVMemory(addr++, fillByte);
    }
    return true;
}

void TableObject::loadEvent(uint8_t* data)
{
    switch (_state)
    {
        case LS_UNLOADED:
            loadEventUnloaded(data);
            break;
        case LS_LOADING:
            loadEventLoading(data);
            break;
        case LS_LOADED:
            loadEventLoaded(data);
            break;
        case LS_ERROR:
            loadEventError(data);
            break;
        default:
            /* do nothing */
            break;
    }
}

void TableObject::loadEventUnloaded(uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_LOAD_COMPLETED:
        case LE_ADDITIONAL_LOAD_CONTROLS:
        case LE_UNLOAD:
            break;
        case LE_START_LOADING:
            loadState(LS_LOADING);
            break;
        default:
            loadState(LS_ERROR);
            _errorCode = E_GOT_UNDEF_LOAD_CMD;
    }
}

void TableObject::loadEventLoading(uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_START_LOADING:
            break;
        case LE_LOAD_COMPLETED:
            _crc = Crc16Citt(_data, _size);
            // print("Calculate CRC: ");
            // println(_crc);
            loadState(LS_LOADED);
            break;
        case LE_UNLOAD:
            loadState(LS_UNLOADED);
            break;
        case LE_ADDITIONAL_LOAD_CONTROLS:
            additionalLoadControls(data);
            break;
        default:
            loadState(LS_ERROR);
            _errorCode = E_GOT_UNDEF_LOAD_CMD;
    }
}

void TableObject::loadEventLoaded(uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_LOAD_COMPLETED:
            break;
        case LE_START_LOADING:
            loadState(LS_LOADING);
            break;
        case LE_UNLOAD:
            loadState(LS_UNLOADED);
            break;
        case LE_ADDITIONAL_LOAD_CONTROLS:
            loadState(LS_ERROR);
            _errorCode = E_INVALID_OPCODE;
            break;
        default:
            loadState(LS_ERROR);
            _errorCode = E_GOT_UNDEF_LOAD_CMD;
    }
}

void TableObject::loadEventError(uint8_t* data)
{
    uint8_t event = data[0];
    switch (event)
    {
        case LE_NOOP:
        case LE_LOAD_COMPLETED:
        case LE_ADDITIONAL_LOAD_CONTROLS:
        case LE_START_LOADING:
            break;
        case LE_UNLOAD:
            loadState(LS_UNLOADED);
            break;
        default:
            loadState(LS_ERROR);
            _errorCode = E_GOT_UNDEF_LOAD_CMD;
    }
}

void TableObject::additionalLoadControls(uint8_t* data)
{
    if (data[1] != 0x0B) // Data Relative Allocation
    {
        loadState(LS_ERROR);
        _errorCode = E_INVALID_OPCODE;
        return;
    }

    size_t size = ((data[2] << 24) | (data[3] << 16) | (data[4] << 8) | data[5]);
    bool doFill = data[6] == 0x1;
    uint8_t fillByte = data[7];
    if (!allocTable(size, doFill, fillByte))
    {
        loadState(LS_ERROR);
        _errorCode = E_MAX_TABLE_LENGTH_EXEEDED;
    }
}

uint8_t* TableObject::data()
{
    return _data;
}

uint32_t TableObject::size()
{
    return _size + METADATA_SIZE;
}

uint32_t TableObject::sizeMetadata()
{
    return METADATA_SIZE;
}

void TableObject::errorCode(ErrorCode errorCode)
{
    _errorCode = errorCode;
}
