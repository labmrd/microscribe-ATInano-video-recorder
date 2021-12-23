

public interface myCLibrary extends Library {
   //Native.addSearchPath( "armdll64.dll" , "C:\\tissueCapture\\" );
   myCLibrary INSTANCE = (myCLibrary)  Native.load("armdll64.dll", myCLibrary.class); // should be in ./code/ dir (on class path)
  //addSearchPath("armdll64.dll", "test");
             //void printf(String format, Object... args);
 
 
   public static class HWND extends PointerType {
      public HWND(Pointer address) {
        super(address);
      }
      public HWND() {
        super();
      }
     }
  //@FieldOrder({ "x", "y", "z"})
  public static class length_3D extends Structure /*<length_3D, length_3D.ByValue, length_3D.ByReference >*/{
  /** C type : length */
  public float x;
  /** C type : length */
  public float y;
  /** C type : length */
  public float z;
  public length_3D() {
    super();
  }
  protected List getFieldOrder() {
         return Arrays.asList("x", "y", "z");
     }

  //protected List<? > getFieldOrder() {
  //  return Arrays.asList("x", "y", "z");
  //}
  /**
   * @param x C type : length<br>
   * @param y C type : length<br>
   * @param z C type : length
   */
  public length_3D(float x, float y, float z) {
    super();
    this.x = x;
    this.y = y;
    this.z = z;
  }
  //public length_3D(Pointer peer) {
  //  super(peer);
  //}
  protected ByReference newByReference() { return new ByReference(); }
  //protected ByValue newByValue() { return new ByValue(); }
  protected length_3D newInstance() { return new length_3D(); }
  //public static length_3D[] newArray(int arrayLength) {
  //  return Structure.newArray(length_3D.class, arrayLength);
  //}
  public static class ByReference extends length_3D implements Structure.ByReference {
    
  };
  //public static class ByValue extends length_3D implements Structure.ByValue {
    
  //};
  }
  int ArmStart(myCLibrary.HWND hwndParent);
  void ArmEnd();
  // Original signature : <code>int ArmConnect(int, long)</code><br>
  // <i>native declaration : line 452</i>  
  int ArmConnect(int port, NativeLong baud);
  // Original signature : <code>void ArmDisconnect()</code><br>
  // <i>native declaration : line 454</i>   
  void ArmDisconnect();
  // Original signature : <code>int ArmSetUpdateEx(int, UINT)</code><br>
  //<i>native declaration : line 468</i>  
  int ArmSetUpdateEx(int type, int minUpdatePeriodms);
   // Original signature : <code>int ArmGetTipPosition(length_3D*)</code><br>
   // <i>native declaration : line 470</i>
  int ArmGetTipPosition(length_3D pPosition);
  // Original signature : <code>int ArmGetTipOrientation(angle_3D*)</code><br>
  //<i>native declaration : line 471</i>  
  int ArmGetTipOrientation(length_3D pAngles); // angle about x, y, z
  // Original signature : <code>int ArmGetTipOrientationUnitVector(angle_3D*)</code><br>
  // <i>native declaration : line 472</i>  
  int ArmGetTipOrientationUnitVector(length_3D pOrientationUnitVector); // tip orientation vector in x,y,z
  
  
  
  public static final int ARM_SUCCESS = (int)0;
  public static final int ARM_FULL = (int)0x0010;
}
