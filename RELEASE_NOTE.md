### version 0.0.1
* �ŏ��̃����[�X�ł��B  
  
### version 0.0.2  

* #### �ύX
  * V3DGraphicsPipelineDesc �\���̂̃����o�ł�����
    vertexElementCount pVertexElements vertexLayoutCount pVertexLayouts
    �� V3DPipelineVertexInputDesc �\���̂Ɉړ����AV3DGraphicsPipelineDesc �\���̂� vertexInput �Ƃ��Đ錾����悤�ɂ��܂����B  
  
  * V3DSwapChainDesc �\���̂̃C���[�W�̎g�p���@�ł��� imageUsageFlags ��ǉ����܂����B
    �C���[�W�̎g�p���@��C�ӂŎw��ł��܂��B  
    �܂��o�͐�̃J���[�A�^�b�`�����g�Ƃ��Ďg�p����͂��Ȃ̂ŁAV3D_IMAGE_USAGE_COLOR_ATTACHMENT �͕K���w�肷�邱�ƂɂȂ�͂��ł��B  
  
  * �W���̃��O��\������񋓒萔�g�ݍ��킹�ł��� V3D_LOG_STANDARD ���� V3D_LOG_PERFORMANCE_WARNING ���O���܂����B  
  
* #### �C��
  * IV3DCommandBuffer::BlitImage �̕s����C�����܂����B  
  
  * IV3DCommandBuffer::ResolveImage ����� IV3DCommandBuffer::ResolveImageView �̕s����C�����܂����B  
  
  * �R�}���h�o�b�t�@�[�̃o���A�n�̃��\�b�h�̕s����C�����܂����B  
  
  * �}���`�X���b�h���Ń��O��\�������ۂɗ�����s����C�����܂����B  
  
  * �T���v���Ŏ��̃C���[�W�̎擾�^�C�~���O���Ԉ���Ă����̂��C�����܂����B  

* ### ����
  * IV3DResource::Map �̑������ł��� size �� V3D_WHOLE_SIZE ���w�肷�邱�ƂŁA�������� offset ���烁�����̏I�[�܂ł��}�b�v�ł���悤�ɂ��܂����B  
  
  * �T���v���ɂ��� multiSample �Ń����_�[�p�X���g�p���� IV3DCommandBuffer::ResolveImageView ���g�p�����}���`�T���v�����O���ł���悤�ɂ��܂����B
  
* ### �ǉ�
  * �T���v���̃C���[�W�̓ǂݍ��ݎ��� IV3DCommandBuffer::BlitImage ���g�p�����~�b�v�}�b�v�𐶐�����@�\��ǉ����܂����B( 2D�C���[�W�Ɍ��肳��܂��B ) 
    
  * �T���v���Ƀ}���`�X���b�h�ŕ`�悷�� multithread ��ǉ����܂����B  
  
  * �V�F�[�_�[�̓��ꉻ�萔�ɑΉ����܂����B V3DPipelineShader �\���̂��m�F���Ă��������B  
