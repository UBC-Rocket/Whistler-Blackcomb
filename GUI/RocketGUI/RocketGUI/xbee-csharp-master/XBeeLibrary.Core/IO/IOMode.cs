﻿/*
 * Copyright 2019, Digi International Inc.
 * Copyright 2014, 2015, Sébastien Rault.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

using System.Collections.Generic;
using System.Linq;

namespace XBeeLibrary.Core.IO
{
	/// <summary>
	/// Enumerates the different Input/Output modes that an IO line can be configured with.
	/// </summary>
	/// <seealso cref="IOLine"/>
	public enum IOMode
	{
		UNKOWN = 0xff,
		DISABLED = 0,
		SPECIAL_FUNCTIONALITY = 1,
		PWM = 2,
		ADC = 20,
		DIGITAL_IN = 3,
		DIGITAL_OUT_LOW = 4,
		DIGITAL_OUT_HIGH = 5
	}

	public static class IOModeExtensions
	{
		private static IDictionary<IOMode, IOModeStruct> lookupTable = new Dictionary<IOMode, IOModeStruct>();

		static IOModeExtensions()
		{
			lookupTable.Add(IOMode.DISABLED, new IOModeStruct(0, "Disabled"));
			lookupTable.Add(IOMode.SPECIAL_FUNCTIONALITY, new IOModeStruct(1, "Firmware special functionality"));
			lookupTable.Add(IOMode.PWM, new IOModeStruct(2, "PWM output"));
			lookupTable.Add(IOMode.ADC, new IOModeStruct(2, "Analog to Digital Converter"));
			lookupTable.Add(IOMode.DIGITAL_IN, new IOModeStruct(3, "Digital input"));
			lookupTable.Add(IOMode.DIGITAL_OUT_LOW, new IOModeStruct(4, "Digital output, Low"));
			lookupTable.Add(IOMode.DIGITAL_OUT_HIGH, new IOModeStruct(5, "Digital output, High"));
		}

		/// <summary>
		/// Gets the IO mode ID.
		/// </summary>
		/// <param name="source"></param>
		/// <returns>IO mode ID.</returns>
		public static int GetId(this IOMode source)
		{
			return lookupTable[source].Id;
		}

		/// <summary>
		/// Gets the IO mode name.
		/// </summary>
		/// <param name="source"></param>
		/// <returns>IO mode name.</returns>
		public static string GetName(this IOMode source)
		{
			return lookupTable[source].Name;
		}

		/// <summary>
		/// Gets the <see cref="IOMode"/> corresponding to the provided mode Id.
		/// </summary>
		/// <param name="source"></param>
		/// <param name="modeID">The ID of the <see cref="IOMode"/> to retrieve.</param>
		/// <returns>The <see cref="IOMode"/> corresponding to the provided mode ID.</returns>
		public static IOMode GetIOMode(this IOMode source, int modeID)
		{
			return GetIOMode(source, modeID, IOLine.UNKNOWN);
		}

		/// <summary>
		/// Gets the <see cref="IOMode"/> corresponding to the provided mode Id and IO line.
		/// </summary>
		/// <param name="source"></param>
		/// <param name="modeID">The ID of the <see cref="IOMode"/> to retrieve.</param>
		/// <param name="ioline">The IO line to retrieve its <see cref="IOMode"/></param>
		/// <returns>The <see cref="IOMode"/> corresponding to the provided mode ID and IO line.</returns>
		public static IOMode GetIOMode(this IOMode source, int modeID, IOLine ioline)
		{
			if (modeID == lookupTable[IOMode.ADC].Id)
			{
				return ioline != IOLine.UNKNOWN && ioline.HasPWMCapability() ? IOMode.PWM : IOMode.ADC;
			}

			return lookupTable.Where(io => io.Value.Id == modeID).Select(io => io.Key).FirstOrDefault();
		}
	}

	struct IOModeStruct
	{
		public int Id { get; private set; }

		public string Name { get; private set; }

		public IOModeStruct(int id, string name)
			: this()
		{
			Id = id;
			Name = name;
		}
	}
}
