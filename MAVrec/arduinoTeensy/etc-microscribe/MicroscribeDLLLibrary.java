package microscrobedll;
import com.ochafik.lang.jnaerator.runtime.LibraryExtractor;
import com.ochafik.lang.jnaerator.runtime.MangledFunctionMapper;
import com.sun.jna.Callback;
import com.sun.jna.Native;
import com.sun.jna.NativeLibrary;
import com.sun.jna.NativeLong;
import com.sun.jna.Pointer;
import com.sun.jna.PointerType;
import com.sun.jna.ptr.FloatByReference;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.win32.StdCallLibrary;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
/**
 * JNA Wrapper for library <b>microscrobeDLL</b><br>
 * This file was autogenerated by <a href="http://jnaerator.googlecode.com/">JNAerator</a>,<br>
 * a tool written by <a href="http://ochafik.com/">Olivier Chafik</a> that <a href="http://code.google.com/p/jnaerator/wiki/CreditsAndLicense">uses a few opensource projects.</a>.<br>
 * For help, please visit <a href="http://nativelibs4java.googlecode.com/">NativeLibs4Java</a> , <a href="http://rococoa.dev.java.net/">Rococoa</a>, or <a href="http://jna.dev.java.net/">JNA</a>.
 */
public interface MicroscrobeDLLLibrary extends StdCallLibrary {
	public static final String JNA_LIBRARY_NAME = LibraryExtractor.getLibraryPath("microscrobeDLL", true, MicroscrobeDLLLibrary.class);
	public static final NativeLibrary JNA_NATIVE_LIB = NativeLibrary.getInstance(MicroscrobeDLLLibrary.JNA_LIBRARY_NAME, MangledFunctionMapper.DEFAULT_OPTIONS);
	public static final MicroscrobeDLLLibrary INSTANCE = (MicroscrobeDLLLibrary)Native.loadLibrary(MicroscrobeDLLLibrary.JNA_LIBRARY_NAME, MicroscrobeDLLLibrary.class, MangledFunctionMapper.DEFAULT_OPTIONS);
	public static final int ARM_CANT_GET_NUMDOF = (int)-13;
	public static final int ARM_3JOINT = (int)0x0004;
	public static final int ARM_DATA_ACCESS_ERROR = (int)-10;
	public static final int MINIMUM_PERIOD_UPDATE = (int)1;
	public static final int ARM_OUTOFMEMORY = (int)-7;
	public static final int MAXIMUM_PERIOD_UPDATE = (int)250;
	public static final int ARM_3DOF = (int)0x0001;
	public static final int ARM_RADIANS = (int)2;
	public static final int PTS_TOO_CLOSE = (int)-17;
	public static final int SPHERE_SUCCESS = (int)0;
	public static final int ARM_INVALID_PARENT = (int)-4;
	public static final int MAX_STYLUS_ROLL_ENCODER = (int)2000;
	public static final int ARM_USING_SERIAL_PORT = (int)0x0004;
	public static final int CANT_BEGIN_HANDLER = (int)0x0008;
	public static final int ARM_MM = (int)2;
	public static final int BAD_PORT_HANDLER = (int)0x0001;
	public static final String ARM_MESSAGE = (String)"MscribeUpdate";
	public static final int TIMED_OUT_HANDLER = (int)0x0010;
	public static final int ARM_FULL = (int)0x0010;
	public static final int NUM_ENCODERS = (int)7;
	public static final int MAX_STRING_SIZE = (int)32;
	public static final String ARM_DISCONNECTED_MESSAGE = (String)"Mscribe device disconnected";
	public static final int CANT_OPEN_HANDLER = (int)0x0002;
	public static final int ARM_VERSION_NUMBER_NOT_AVAILABLE = (int)-11;
	public static final int ARM_INVALID_PARAM = (int)-9;
	public static final int DEFAULT_PERIOD_UPDATE = (int)20;
	public static final int NOT_ENOUGH_POINTS = (int)-14;
	public static final int ARM_ALREADY_USED = (int)-1;
	public static final int ARM_DEGREES = (int)1;
	public static final int ARM_BUFFER_TOO_SMALL = (int)-8;
	public static final int ARM_USING_USB_PORT = (int)0x0002;
	public static final int NO_HCI_HANDLER = (int)0x0004;
	public static final int ARM_NOT_ACCESSED = (int)-2;
	public static final int SERIAL_NUMBER_DISCRIMINATOR_ENABLED = (int)1;
	public static final int ARM_CANT_CONNECT = (int)-3;
	public static final int ARM_BAD_PORTBAUD = (int)-6;
	public static final int ARM_6JOINT = (int)0x0008;
	public static final int NOT_CONVERGED = (int)-16;
	public static final int MAX_PACKET_SIZE = (int)42;
	public static final int ARM_NOT_CONNECTED = (int)-5;
	public static final int NUM_DOF = (int)6;
	public static final int ARM_SUCCESS = (int)0;
	public static final int NUM_THETA_PARAM = (int)7;
	public static final int ARM_INCHES = (int)1;
	public static final int PARAM_BLOCK_SIZE = (int)40;
	public static final int ARM_WRONG_ERROR_HANDLER_TYPE = (int)-12;
	public static final int ARM_6DOF = (int)0x0002;
	public static final int NUM_BUTTONS = (int)7;
	public static final int BAD_PACKET_HANDLER = (int)0x0020;
	public static final int ARM_CONNECTED = (int)0x0001;
	public static final int INVALID_ARGUMENTS = (int)-15;
	public static final int ARM_BUTTON_1 = (int)0x0001;
	public static final int NUM_ANALOGS = (int)8;
	public static final int EXT_PARAM_BLOCK_SIZE = (int)10;
	public static final int ARM_BUTTON_5 = (int)0x0010;
	public static final int ARM_BUTTON_4 = (int)0x0008;
	public static final int ARM_BUTTON_3 = (int)0x0004;
	public static final int ARM_BUTTON_2 = (int)0x0002;
	public static final int ARM_BUTTON_7 = (int)0x0040;
	public static final int ARM_BUTTON_6 = (int)0x0020;
	public interface PERRORHANDLER extends Callback {
		Pointer apply(hci_rec hci, Pointer condition);
	};
	/**
	 * ****************************************************************************<br>
	 * Original signature : <code>int ArmStart(HWND)</code><br>
	 * <i>native declaration : line 448</i>
	 */
	int ArmStart(MicroscrobeDLLLibrary.HWND hwndParent);
	/**
	 * Original signature : <code>void ArmEnd()</code><br>
	 * <i>native declaration : line 449</i>
	 */
	void ArmEnd();
	/**
	 * Original signature : <code>int ArmConnectSN(int, long, char*)</code><br>
	 * <i>native declaration : line 451</i><br>
	 * @deprecated use the safer methods {@link #ArmConnectSN(int, com.sun.jna.NativeLong, java.nio.ByteBuffer)} and {@link #ArmConnectSN(int, com.sun.jna.NativeLong, com.sun.jna.Pointer)} instead
	 */
	@Deprecated 
	int ArmConnectSN(int port, NativeLong baud, Pointer serialNumber);
	/**
	 * Original signature : <code>int ArmConnectSN(int, long, char*)</code><br>
	 * <i>native declaration : line 451</i>
	 */
	int ArmConnectSN(int port, NativeLong baud, ByteBuffer serialNumber);
	/**
	 * Original signature : <code>int ArmConnect(int, long)</code><br>
	 * <i>native declaration : line 452</i>
	 */
	int ArmConnect(int port, NativeLong baud);
	/**
	 * Original signature : <code>void ArmDisconnect()</code><br>
	 * <i>native declaration : line 454</i>
	 */
	void ArmDisconnect();
	/**
	 * Original signature : <code>int ArmReconnect()</code><br>
	 * <i>native declaration : line 455</i>
	 */
	int ArmReconnect();
	/**
	 * Original signature : <code>int ArmSetBckgUpdate(int)</code><br>
	 * <i>native declaration : line 456</i>
	 */
	int ArmSetBckgUpdate(int type);
	/**
	 * Original signature : <code>int ArmSetUpdate(int)</code><br>
	 * <i>native declaration : line 457</i>
	 */
	int ArmSetUpdate(int type);
	/**
	 * Original signature : <code>void* ArmGetArmRec()</code><br>
	 * <i>native declaration : line 459</i>
	 */
	Pointer ArmGetArmRec();
	/**
	 * Original signature : <code>void ArmSetLengthUnits(int)</code><br>
	 * <i>native declaration : line 460</i>
	 */
	void ArmSetLengthUnits(int type);
	/**
	 * Original signature : <code>void ArmSetAngleUnits(int)</code><br>
	 * <i>native declaration : line 461</i>
	 */
	void ArmSetAngleUnits(int type);
	/**
	 * Original signature : <code>void arm_calc_stylus_3DOF(arm_rec*)</code><br>
	 * <i>native declaration : line 463</i>
	 */
	void arm_calc_stylus_3DOF(arm_rec arm);
	/**
	 * Original signature : <code>void arm_calc_joints(arm_rec*)</code><br>
	 * <i>native declaration : line 464</i>
	 */
	void arm_calc_joints(arm_rec arm);
	/**
	 * Original signature : <code>void arm_calc_params(arm_rec*)</code><br>
	 * <i>native declaration : line 465</i>
	 */
	void arm_calc_params(arm_rec arm);
	/**
	 * New functions for ArmDll32 2.0<br>
	 * Original signature : <code>int ArmSetUpdateEx(int, UINT)</code><br>
	 * <i>native declaration : line 468</i>
	 */
	int ArmSetUpdateEx(int type, int minUpdatePeriodms);
	/**
	 * Original signature : <code>void ArmCustomTip(float)</code><br>
	 * <i>native declaration : line 469</i>
	 */
	void ArmCustomTip(float delta);
	/**
	 * Original signature : <code>int ArmGetTipPosition(length_3D*)</code><br>
	 * <i>native declaration : line 470</i>
	 */
	int ArmGetTipPosition(length_3D pPosition);
	/**
	 * Original signature : <code>int ArmGetTipOrientation(angle_3D*)</code><br>
	 * <i>native declaration : line 471</i>
	 */
	int ArmGetTipOrientation(angle_3D pAngles);
	/**
	 * Original signature : <code>int ArmGetTipOrientationUnitVector(angle_3D*)</code><br>
	 * <i>native declaration : line 472</i>
	 */
	int ArmGetTipOrientationUnitVector(angle_3D pOrientationUnitVector);
	/**
	 * Original signature : <code>int ArmGetProductName(char*, UINT)</code><br>
	 * <i>native declaration : line 473</i><br>
	 * @deprecated use the safer methods {@link #ArmGetProductName(java.nio.ByteBuffer, int)} and {@link #ArmGetProductName(com.sun.jna.Pointer, int)} instead
	 */
	@Deprecated 
	int ArmGetProductName(Pointer szProductName, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetProductName(char*, UINT)</code><br>
	 * <i>native declaration : line 473</i>
	 */
	int ArmGetProductName(ByteBuffer szProductName, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetSerialNumber(char*, UINT)</code><br>
	 * <i>native declaration : line 474</i><br>
	 * @deprecated use the safer methods {@link #ArmGetSerialNumber(java.nio.ByteBuffer, int)} and {@link #ArmGetSerialNumber(com.sun.jna.Pointer, int)} instead
	 */
	@Deprecated 
	int ArmGetSerialNumber(Pointer szSerialNumber, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetSerialNumber(char*, UINT)</code><br>
	 * <i>native declaration : line 474</i>
	 */
	int ArmGetSerialNumber(ByteBuffer szSerialNumber, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetModelName(char*, UINT)</code><br>
	 * <i>native declaration : line 475</i><br>
	 * @deprecated use the safer methods {@link #ArmGetModelName(java.nio.ByteBuffer, int)} and {@link #ArmGetModelName(com.sun.jna.Pointer, int)} instead
	 */
	@Deprecated 
	int ArmGetModelName(Pointer szModelName, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetModelName(char*, UINT)</code><br>
	 * <i>native declaration : line 475</i>
	 */
	int ArmGetModelName(ByteBuffer szModelName, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetVersion(char*, char*, UINT)</code><br>
	 * <i>native declaration : line 476</i><br>
	 * @deprecated use the safer methods {@link #ArmGetVersion(java.nio.ByteBuffer, java.nio.ByteBuffer, int)} and {@link #ArmGetVersion(com.sun.jna.Pointer, com.sun.jna.Pointer, int)} instead
	 */
	@Deprecated 
	int ArmGetVersion(Pointer szArmDllVersionNumber, Pointer szFirmwareVersionNumber, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetVersion(char*, char*, UINT)</code><br>
	 * <i>native declaration : line 476</i>
	 */
	int ArmGetVersion(ByteBuffer szArmDllVersionNumber, ByteBuffer szFirmwareVersionNumber, int uiBufferLength);
	/**
	 * Original signature : <code>int ArmGetNumButtons(int*)</code><br>
	 * <i>native declaration : line 477</i><br>
	 * @deprecated use the safer methods {@link #ArmGetNumButtons(java.nio.IntBuffer)} and {@link #ArmGetNumButtons(com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetNumButtons(IntByReference piNumButtons);
	/**
	 * Original signature : <code>int ArmGetNumButtons(int*)</code><br>
	 * <i>native declaration : line 477</i>
	 */
	int ArmGetNumButtons(IntBuffer piNumButtons);
	/**
	 * Original signature : <code>int ArmGetButtonsState(DWORD*)</code><br>
	 * <i>native declaration : line 478</i><br>
	 * @deprecated use the safer methods {@link #ArmGetButtonsState(java.nio.IntBuffer)} and {@link #ArmGetButtonsState(com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetButtonsState(IntByReference pdwButtonsState);
	/**
	 * Original signature : <code>int ArmGetButtonsState(DWORD*)</code><br>
	 * <i>native declaration : line 478</i>
	 */
	int ArmGetButtonsState(IntBuffer pdwButtonsState);
	/**
	 * Original signature : <code>int ArmGetDeviceStatus(device_status*)</code><br>
	 * <i>native declaration : line 479</i>
	 */
	int ArmGetDeviceStatus(device_status pDeviceStatus);
	/**
	 * Original signature : <code>int ArmSetErrorHandlerFunction(int, PERRORHANDLER)</code><br>
	 * <i>native declaration : line 480</i>
	 */
	int ArmSetErrorHandlerFunction(int iErrorType, MicroscrobeDLLLibrary.PERRORHANDLER pErrorHandlerFunction);
	/**
	 * Original signature : <code>int ArmGetNumDOF(int*)</code><br>
	 * <i>native declaration : line 481</i><br>
	 * @deprecated use the safer methods {@link #ArmGetNumDOF(java.nio.IntBuffer)} and {@link #ArmGetNumDOF(com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetNumDOF(IntByReference piNumDOF);
	/**
	 * Original signature : <code>int ArmGetNumDOF(int*)</code><br>
	 * <i>native declaration : line 481</i>
	 */
	int ArmGetNumDOF(IntBuffer piNumDOF);
	/**
	 * Original signature : <code>int ArmGetEncoderCount(int*)</code><br>
	 * <i>native declaration : line 482</i><br>
	 * @deprecated use the safer methods {@link #ArmGetEncoderCount(java.nio.IntBuffer)} and {@link #ArmGetEncoderCount(com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetEncoderCount(IntByReference piEncoderValues);
	/**
	 * Original signature : <code>int ArmGetEncoderCount(int*)</code><br>
	 * <i>native declaration : line 482</i>
	 */
	int ArmGetEncoderCount(IntBuffer piEncoderValues);
	/**
	 * Original signature : <code>int ArmGetJointAngles(int, angle*)</code><br>
	 * <i>native declaration : line 483</i><br>
	 * @deprecated use the safer methods {@link #ArmGetJointAngles(int, java.nio.FloatBuffer)} and {@link #ArmGetJointAngles(int, com.sun.jna.ptr.FloatByReference)} instead
	 */
	@Deprecated 
	int ArmGetJointAngles(int iUnitID, FloatByReference piJointAngle);
	/**
	 * Original signature : <code>int ArmGetJointAngles(int, angle*)</code><br>
	 * <i>native declaration : line 483</i>
	 */
	int ArmGetJointAngles(int iUnitID, FloatBuffer piJointAngle);
	/**
	 * Original signature : <code>int ArmSetHomeEncoderOffset(int*)</code><br>
	 * <i>native declaration : line 484</i><br>
	 * @deprecated use the safer methods {@link #ArmSetHomeEncoderOffset(java.nio.IntBuffer)} and {@link #ArmSetHomeEncoderOffset(com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmSetHomeEncoderOffset(IntByReference piEncoderOffsets);
	/**
	 * Original signature : <code>int ArmSetHomeEncoderOffset(int*)</code><br>
	 * <i>native declaration : line 484</i>
	 */
	int ArmSetHomeEncoderOffset(IntBuffer piEncoderOffsets);
	/**
	 * Original signature : <code>int ArmSetTipPositionOffset(int, length, length, length)</code><br>
	 * <i>native declaration : line 485</i>
	 */
	int ArmSetTipPositionOffset(int iUnitID, float Xoffset, float Yoffset, float ZOffset);
	/**
	 * Original signature : <code>int ArmSetSoftHome()</code><br>
	 * <i>native declaration : line 486</i>
	 */
	int ArmSetSoftHome();
	/**
	 * Original signature : <code>int ArmGetFullTip(length_3D*, angle_3D*, angle_3D*, angle_3D*, DWORD*)</code><br>
	 * <i>native declaration : line 488</i><br>
	 * @deprecated use the safer methods {@link #ArmGetFullTip(microscrobedll.length_3D, microscrobedll.angle_3D, microscrobedll.angle_3D, microscrobedll.angle_3D, java.nio.IntBuffer)} and {@link #ArmGetFullTip(microscrobedll.length_3D, microscrobedll.angle_3D, microscrobedll.angle_3D, microscrobedll.angle_3D, com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetFullTip(length_3D pPosition, angle_3D vecT0, angle_3D vecT1, angle_3D vecT2, IntByReference pdwButtonsState);
	/**
	 * Original signature : <code>int ArmGetFullTip(length_3D*, angle_3D*, angle_3D*, angle_3D*, DWORD*)</code><br>
	 * <i>native declaration : line 488</i>
	 */
	int ArmGetFullTip(length_3D pPosition, angle_3D vecT0, angle_3D vecT1, angle_3D vecT2, IntBuffer pdwButtonsState);
	/**
	 * custom tip support functions<br>
	 * Original signature : <code>int ArmSetTipProfile(int)</code><br>
	 * <i>native declaration : line 493</i>
	 */
	int ArmSetTipProfile(int _tipNumber);
	/**
	 * Original signature : <code>int ArmGetTipProfile(int*)</code><br>
	 * <i>native declaration : line 494</i><br>
	 * @deprecated use the safer methods {@link #ArmGetTipProfile(java.nio.IntBuffer)} and {@link #ArmGetTipProfile(com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetTipProfile(IntByReference _tipNumber);
	/**
	 * Original signature : <code>int ArmGetTipProfile(int*)</code><br>
	 * <i>native declaration : line 494</i>
	 */
	int ArmGetTipProfile(IntBuffer _tipNumber);
	/**
	 * Original signature : <code>int ArmSaveTipProfile(int, int*, float*, char*)</code><br>
	 * <i>native declaration : line 495</i><br>
	 * @deprecated use the safer methods {@link #ArmSaveTipProfile(int, java.nio.IntBuffer, java.nio.FloatBuffer, java.nio.ByteBuffer)} and {@link #ArmSaveTipProfile(int, com.sun.jna.ptr.IntByReference, com.sun.jna.ptr.FloatByReference, com.sun.jna.Pointer)} instead
	 */
	@Deprecated 
	int ArmSaveTipProfile(int _tipNumber, IntByReference _homeDelta, FloatByReference _tipOffset, Pointer _name);
	/**
	 * Original signature : <code>int ArmSaveTipProfile(int, int*, float*, char*)</code><br>
	 * <i>native declaration : line 495</i>
	 */
	int ArmSaveTipProfile(int _tipNumber, IntBuffer _homeDelta, FloatBuffer _tipOffset, ByteBuffer _name);
	/**
	 * Original signature : <code>int ArmGetTipProfileHomeOffset(int, int*)</code><br>
	 * <i>native declaration : line 499</i><br>
	 * @deprecated use the safer methods {@link #ArmGetTipProfileHomeOffset(int, java.nio.IntBuffer)} and {@link #ArmGetTipProfileHomeOffset(int, com.sun.jna.ptr.IntByReference)} instead
	 */
	@Deprecated 
	int ArmGetTipProfileHomeOffset(int _tipNumber, IntByReference _homeDelta);
	/**
	 * Original signature : <code>int ArmGetTipProfileHomeOffset(int, int*)</code><br>
	 * <i>native declaration : line 499</i>
	 */
	int ArmGetTipProfileHomeOffset(int _tipNumber, IntBuffer _homeDelta);
	/**
	 * Original signature : <code>int ArmGetTipProfilePositionOffset(int, float*)</code><br>
	 * <i>native declaration : line 501</i><br>
	 * @deprecated use the safer methods {@link #ArmGetTipProfilePositionOffset(int, java.nio.FloatBuffer)} and {@link #ArmGetTipProfilePositionOffset(int, com.sun.jna.ptr.FloatByReference)} instead
	 */
	@Deprecated 
	int ArmGetTipProfilePositionOffset(int _tipNumber, FloatByReference _tipOffset);
	/**
	 * Original signature : <code>int ArmGetTipProfilePositionOffset(int, float*)</code><br>
	 * <i>native declaration : line 501</i>
	 */
	int ArmGetTipProfilePositionOffset(int _tipNumber, FloatBuffer _tipOffset);
	/**
	 * Original signature : <code>int ArmGetTipNameData(int, char*)</code><br>
	 * <i>native declaration : line 503</i><br>
	 * @deprecated use the safer methods {@link #ArmGetTipNameData(int, java.nio.ByteBuffer)} and {@link #ArmGetTipNameData(int, com.sun.jna.Pointer)} instead
	 */
	@Deprecated 
	int ArmGetTipNameData(int _tipNumber, Pointer _name);
	/**
	 * Original signature : <code>int ArmGetTipNameData(int, char*)</code><br>
	 * <i>native declaration : line 503</i>
	 */
	int ArmGetTipNameData(int _tipNumber, ByteBuffer _name);
	/**
	 * Original signature : <code>int ArmGenerateTipPositionOffset(length_3D*, angle_3D*, int, length_3D&)</code><br>
	 * <i>native declaration : line 505</i>
	 */
	int ArmGenerateTipPositionOffset(length_3D _points, angle_3D _orientations, int _numPoints, length_3D _offset);
	/**
	 * Original signature : <code>int ArmGenerateTipPositionOffsetEx(length_3D*, angle_3D*, int, length_3D&, angle_3D*)</code><br>
	 * <i>native declaration : line 510</i>
	 */
	int ArmGenerateTipPositionOffsetEx(length_3D _points, angle_3D _orientations, int _numPoints, length_3D _offset, angle_3D _unitOrientations);
	public static class HWND extends PointerType {
		public HWND(Pointer address) {
			super(address);
		}
		public HWND() {
			super();
		}
	};
}