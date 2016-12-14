/**
 * 
 */
package org.marpa;

import java.nio.ByteBuffer;
import java.util.logging.Logger;

/**
 * @author Jean-Damien Durand
 *
 */
public class ESLIFOption {
	private ByteBuffer _genericLoggerp        = null;
	private ByteBuffer _genericLoggerContextp = null;
	private Logger     _logger                = null;
	
	public ESLIFOption() {
	}
	
	public ESLIFOption(Logger logger) {
		set_logger(logger);
	}

	@SuppressWarnings("unused")
	private ByteBuffer get_genericLoggerp() {
		return _genericLoggerp;
	}

	@SuppressWarnings("unused")
	private void set_genericLoggerp(ByteBuffer _genericLoggerp) {
		this._genericLoggerp = _genericLoggerp;
	}

	@SuppressWarnings("unused")
	private ByteBuffer get_genericLoggerContextp() {
		return _genericLoggerContextp;
	}

	@SuppressWarnings("unused")
	private void set_genericLoggerContextp(ByteBuffer _genericLoggerContextp) {
		this._genericLoggerContextp = _genericLoggerContextp;
	}

	@SuppressWarnings("unused")
	private Logger get_logger() {
		return _logger;
	}

	private void set_logger(Logger _logger) {
		this._logger = _logger;
	}
	
}
