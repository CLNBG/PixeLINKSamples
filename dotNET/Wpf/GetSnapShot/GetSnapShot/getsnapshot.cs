
using PixeLINK;
using System.IO;
using System;

namespace GetSnapshot
{

	public class SnapshotHelper
	{
		private int m_hCamera;		
	
		public SnapshotHelper(int hCamera) 
		{
			m_hCamera = hCamera;
		}
		
		public bool GetSnapshot(ImageFormat imageFormat, string filename, string directoryName)
		{
			int rawImageSize = DetermineRawImageSize();
			byte[] buf = new byte[rawImageSize];

			Api.SetStreamState(m_hCamera, StreamState.Start);

			FrameDescriptor frameDesc = new FrameDescriptor();
			
			ReturnCode rc = Api.GetNextFrame(m_hCamera, buf.Length, buf, ref frameDesc);

			if (2 == Api.BytesPerPixel(GetPixelFormat()))
			{	
				// 			
				//  If we want to interpret the data as U16 pixel data, we need to re-cast the framebuffer
				//  as an array of U16s. In order to do this in C#, we need to tell .NET this is unmanaged
				//  space; a buffer that needs to fixed in a location and will not be moved.
				unsafe
				{
					fixed (byte* pBuffer8 = buf)
					{
						ushort* pBuffer16 = (ushort*)pBuffer8;
						/* Prints 16 bit value of first three pixels to Console. 
						Console.Out.WriteLine("The first 3 pixels are " + SwapEndiannessUshort(pBuffer16[0])
											   + " " + SwapEndiannessUshort(pBuffer16[1])
											   + " " + SwapEndiannessUshort(pBuffer16[2])
											   + "\n");
						*/
					}
				}
			}

			Api.SetStreamState(m_hCamera, StreamState.Stop);

			// How big a buffer do we need for the converted image?
			int destBufferSize = 0;
			rc = Api.FormatImage(buf, ref frameDesc, imageFormat, null, ref destBufferSize);
			byte[] dstBuf = new byte[destBufferSize];
			rc = Api.FormatImage(buf, ref frameDesc, imageFormat, dstBuf, ref destBufferSize);


            filename = directoryName + "\\" + filename;

            // Save the data to a binary file
            FileStream fStream = new FileStream(filename, FileMode.OpenOrCreate);
			BinaryWriter bw = new BinaryWriter(fStream);
			bw.Write(dstBuf);
			bw.Close();
			fStream.Close();

			return true;
		}

		private ReturnCode GetNextFrame(ref FrameDescriptor frameDesc, byte[] buf)
		{
			ReturnCode rc = ReturnCode.UnknownError;
			const int NUM_TRIES = 4;
			for(int i=0; i < NUM_TRIES; i++) 
			{
				rc = Api.GetNextFrame(m_hCamera, buf.Length, buf, ref frameDesc);
				if (Api.IsSuccess(rc)) 
				{
					return rc;
				}
			}
			return rc;
		}

		//
		// The 16 bit format (such as MONO16) are represented in big-endian order as per the Pixelink SDK documentation. 
		// Intel computers running Windows are little-endian. Thus, the bytes need to be swapped to treat them as 16 bits.
		//
		private ushort SwapEndiannessUshort(ushort rawData)
		{
			return (ushort)(rawData >> 8 | rawData << 8);
		}

		private PixelFormat GetPixelFormat() 
		{
			FeatureFlags flags = 0;
			int numParms = 1;
			float[] parms = new float[numParms];
			Api.GetFeature(m_hCamera, Feature.PixelFormat,  ref flags, ref numParms, parms);
			return (PixelFormat)parms[0];
		}

		private int GetNumPixels()
		{
			// Query the ROI
			FeatureFlags flags = 0;
			int numParms = 4;
			float[] parms = new float[numParms];
			Api.GetFeature(m_hCamera, Feature.Roi,  ref flags, ref numParms, parms);

			int width = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiWidth]);
			int height = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiHeight]);

			// Take the decimation (pixel addressing value) into account
			numParms = 2;
			Api.GetFeature(m_hCamera, Feature.PixelAddressing, ref flags, ref numParms, parms);
			int pixelAddressingValue = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.PixelAddressingValue]);

			return (width / pixelAddressingValue) * (height / pixelAddressingValue);

		}

		//
		// This assumes there's no pixel addressing (decimation)
		//
		private int DetermineRawImageSize() 
		{
			float bytesPerPixel = Api.BytesPerPixel(GetPixelFormat());
			return (int) (bytesPerPixel * (float)GetNumPixels());
			
		}
	}

}