#include "domedecahedron.h"


// (1.0 + sqrt(5.0)) / 2.0
#define PHI (1.618033988749895f)
// 2.0 / (1.0 + sqrt(5.0))
#define IPHI (0.6180339887498948f)

// These vertices are given in the standard threading order
//    { 0.0f, -IPHI,   PHI},
//    { 0.0f,  IPHI,   PHI},
//    {-1.0f,  1.0f,  1.0f},
//    { -PHI,  0.0f,  IPHI},
//    {-1.0f, -1.0f,  1.0f},
//    {-IPHI,  -PHI,  0.0f},
//    {-1.0f, -1.0f, -1.0f},
//    { -PHI,  0.0f, -IPHI},
//    {-1.0f,  1.0f, -1.0f},
//    {-IPHI,   PHI,  0.0f},
//    { IPHI,   PHI,  0.0f},
//    { 1.0f,  1.0f,  1.0f},
//    {  PHI,  0.0f,  IPHI},
//    { 1.0f, -1.0f,  1.0f},
//    { IPHI,  -PHI,  0.0f},
//    { 1.0f, -1.0f, -1.0f},
//    { 0.0f, -IPHI,  -PHI},
//    { 0.0f,  IPHI,  -PHI},
//    { 1.0f,  1.0f, -1.0f},
//    {  PHI,  0.0f, -IPHI},

// normalized to unit radius
//    {0.0f, -0.35682208977309f, 0.9341723589627159f},
//    {0.0f, 0.35682208977309f, 0.9341723589627159f},
//    {-0.5773502691896258f, 0.5773502691896258f, 0.5773502691896258f},
//    {-0.9341723589627159f, 0.0f, 0.35682208977309f},
//    {-0.5773502691896258f, -0.5773502691896258f, 0.5773502691896258f},
//    {-0.35682208977309f, -0.9341723589627159f, 0.0f},
//    {-0.5773502691896258f, -0.5773502691896258f, -0.5773502691896258f},
//    {-0.9341723589627159f, 0.0f, -0.35682208977309f},
//    {-0.5773502691896258f, 0.5773502691896258f, -0.5773502691896258f},
//    {-0.35682208977309f, 0.9341723589627159f, 0.0f},
//    {0.35682208977309f, 0.9341723589627159f, 0.0f},
//    {0.5773502691896258f, 0.5773502691896258f, 0.5773502691896258f},
//    {0.9341723589627159f, 0.0f, 0.35682208977309f},
//    {0.5773502691896258f, -0.5773502691896258f, 0.5773502691896258f},
//    {0.35682208977309f, -0.9341723589627159f, 0.0f},
//    {0.5773502691896258f, -0.5773502691896258f, -0.5773502691896258f},
//    {0.0f, -0.35682208977309f, -0.9341723589627159f},
//    {0.0f, 0.35682208977309f, -0.9341723589627159f},
//    {0.5773502691896258f, 0.5773502691896258f, -0.5773502691896258f},
//    {0.9341723589627159f, 0.0f, -0.35682208977309f},

// normalized and rotated so the top face is in a plane
vertex_t const ddh_dodecahedron_vertex_coords[
        DDH_VERTICES_PER_DODECAHEDRON] = {
    {0.491123473188f, -0.356822089773f, 0.794654472292f},
    {0.491123473188f, 0.356822089773f, 0.794654472292f},
    {-0.187592474085f, 0.57735026919f, 0.794654472292f},
    {-0.607061998207f, 0.0f, 0.794654472292f},
    {-0.187592474085f, -0.57735026919f, 0.794654472292f},
    {-0.303530999103f, -0.934172358963f, 0.187592474085f},
    {-0.794654472292f, -0.57735026919f, -0.187592474085f},
    {-0.982246946377f, 0.0f, 0.187592474085f},
    {-0.794654472292f, 0.57735026919f, -0.187592474085f},
    {-0.303530999103f, 0.934172358963f, 0.187592474085f},
    {0.303530999103f, 0.934172358963f, -0.187592474085f},
    {0.794654472292f, 0.57735026919f, 0.187592474085f},
    {0.982246946377f, 0.0f, -0.187592474085f},
    {0.794654472292f, -0.57735026919f, 0.187592474085f},
    {0.303530999103f, -0.934172358963f, -0.187592474085f},
    {0.187592474085f, -0.57735026919f, -0.794654472292f},
    {-0.491123473188f, -0.356822089773f, -0.794654472292f},
    {-0.491123473188f, 0.356822089773f, -0.794654472292f},
    {0.187592474085f, 0.57735026919f, -0.794654472292f},
    {0.607061998207f, 0.0f, -0.794654472292f},
};


vertex_t const ddh_vertex_coords[DDH_TOTAL_VERTICES] = {
    {0.02438997476245247f, -0.23205510891926384f, 1.0108745973749755f},
    {0.0f, -4.163336342344337e-17f, 1.1f},
    {-0.2131606067832736f, 0.09490521671768665f, 1.0108745973749755f},
    {-0.3205111320754356f, -0.07849524256037198f, 0.8666666666666667f},
    {-0.17369679863287332f, -0.28056783677674096f, 0.8666666666666667f},
    {-0.09227669190421423f, -0.3168184038532227f, 0.6333333333333333f},
    {-0.1887706320208211f, -0.137149892201577f, 0.4891254026250245f},
    {-0.32982727344994034f, 0.010141921783727956f, 0.6333333333333333f},
    {-0.22823444017122135f, 0.2383231612928506f, 0.6333333333333333f},
    {-0.15613047481706696f, 0.290709758560469f, 0.8666666666666667f},
    {0.09227669190421423f, 0.3168184038532227f, 0.8666666666666667f},
    {0.1887706320208211f, 0.137149892201577f, 1.0108745973749755f},
    {0.32982727344994034f, -0.010141921783727956f, 0.8666666666666667f},
    {0.22823444017122135f, -0.2383231612928506f, 0.8666666666666667f},
    {0.15613047481706696f, -0.290709758560469f, 0.6333333333333333f},
    {0.2131606067832736f, -0.09490521671768665f, 0.4891254026250245f},
    {0.0f, 4.163336342344337e-17f, 0.3999999999999999f},
    {-0.02438997476245247f, 0.23205510891926384f, 0.48912540262502446f},
    {0.17369679863287332f, 0.28056783677674096f, 0.6333333333333333f},
    {0.3205111320754356f, 0.07849524256037198f, 0.6333333333333333f},
    // group 0, dodecahedron 0
    {0.01463737167921203f, 0.26041013586433176f, 0.42597782959169406f},
    {0.03902734644166456f, 0.028355026945068f, 0.3368524269666695f},
    {0.2521879532249382f, -0.06655018977261865f, 0.425977829591694f},
    {0.35953847851710014f, 0.10685026950543998f, 0.5701857603000029f},
    {0.2127241450745379f, 0.308922863721809f, 0.5701857603000028f},
    {0.3400037162458591f, 0.49680262237364703f, 0.46583592135001267f},
    {0.5654811507458093f, 0.4108461048022408f, 0.40134417415834106f},
    {0.5775542977915853f, 0.16984229673669643f, 0.46583592135001267f},
    {0.6049449588962095f, 0.03537305130781321f, 0.2571362434500323f},
    {0.403857499158712f, -0.11072554004004462f, 0.23250258801667922f},
    {0.284433826820774f, -0.04312219125255898f, 0.023802910116698772f},
    {0.05895639232082389f, 0.04283432631884726f, 0.0882946573083705f},
    {0.046883245275047836f, 0.28383813438439165f, 0.023802910116698772f},
    {0.0194925841704236f, 0.41830737981327487f, 0.23250258801667922f},
    {0.22058004390792113f, 0.5644059711611327f, 0.2571362434500323f},
    {0.37224958984169493f, 0.5202306208937068f, 0.06366100187501744f},
    {0.5854101966249685f, 0.42532540417602005f, 0.152786404500042f},
    {0.609800171387421f, 0.19327029525675632f, 0.06366100187501744f},
    {0.4117133979920952f, 0.1447575673992791f, -0.08054692883329129f},
    {0.264899064549533f, 0.3468301616156481f, -0.0805469288332914f},
    // group 0, dodecahedron 1
    {0.6488275178290854f, 0.22162532220182424f, 0.0005134288416870358f},
    {0.624437543066633f, 0.453680431121088f, 0.08963883146671159f},
    {0.41127693628335943f, 0.5485856478387747f, 0.0005134288416870358f},
    {0.30392641099119744f, 0.3751851885607161f, -0.1436945018666218f},
    {0.45074074443375967f, 0.17311259434434711f, -0.1436945018666218f},
    {0.5321608511624187f, 0.1368620272678654f, -0.3770278351999552f},
    {0.4356669110458119f, 0.3165305389195111f, -0.521235765908264f},
    {0.2946102696166927f, 0.46382235290481605f, -0.3770278351999552f},
    {0.39620310289541166f, 0.6920035924139387f, -0.3770278351999552f},
    {0.468307068249566f, 0.744390189681557f, -0.1436945018666218f},
    {0.7167142349708473f, 0.7704988349743107f, -0.1436945018666218f},
    {0.8132081750874541f, 0.590830323322665f, 0.0005134288416870358f},
    {0.9542648165165734f, 0.4435385093373601f, -0.1436945018666218f},
    {0.8526719832378544f, 0.21535726982823747f, -0.1436945018666218f},
    {0.7805680178837f, 0.1629706725606191f, -0.3770278351999552f},
    {0.8375981498499065f, 0.35877521440340143f, -0.521235765908264f},
    {0.624437543066633f, 0.45368043112108813f, -0.6103611685332886f},
    {0.6000475683041806f, 0.685735540040352f, -0.521235765908264f},
    {0.7981343416995064f, 0.734248267897829f, -0.3770278351999552f},
    {0.9449486751420686f, 0.53217567368146f, -0.3770278351999552f},
    // group 0, dodecahedron 2
    {0.639074914745845f, 0.7140905669854198f, -0.5843833389415947f},
    {0.6634648895082975f, 0.4820354580661561f, -0.6735087415666192f},
    {0.8766254962915712f, 0.38713024134846946f, -0.5843833389415947f},
    {0.9839760215837332f, 0.5605307006265281f, -0.4401754082332858f},
    {0.8371616881411709f, 0.7626032948428971f, -0.4401754082332858f},
    {0.9644412593124921f, 0.9504830534947352f, -0.544525247183276f},
    {1.1899186938124422f, 0.864526535923329f, -0.6090169943749475f},
    {1.2019918408582182f, 0.6235227278577845f, -0.544525247183276f},
    {1.2293825019628426f, 0.48905348242890134f, -0.7532249250832563f},
    {1.028295042225345f, 0.3429548910810435f, -0.7778585805166094f},
    {0.908871369887407f, 0.41055823986852913f, -0.9865582584165897f},
    {0.683393935387457f, 0.49651475743993534f, -0.9220665112249182f},
    {0.6713207883416809f, 0.7375185655054798f, -0.9865582584165897f},
    {0.6439301272370566f, 0.871987810934363f, -0.7778585805166094f},
    {0.8450175869745542f, 1.0180864022822207f, -0.7532249250832563f},
    {0.996687132908328f, 0.9739110520147949f, -0.9467001666582711f},
    {1.2098477396916016f, 0.8790058352971082f, -0.8575747640332465f},
    {1.2342377144540542f, 0.6469507263778445f, -0.9467001666582711f},
    {1.0361509410587282f, 0.5984379985203672f, -1.09090809736658f},
    {0.889336607616166f, 0.8005105927367362f, -1.09090809736658f},
    // group 1, dodecahedron 0
    {-0.23284047890107445f, -0.11752873221333338f, 0.425977829591694f},
    {-0.04406984688025345f, 0.019621159988243764f, 0.3368524269666695f},
    {-0.06845982164270603f, 0.2516762689075076f, 0.42597782959169406f},
    {-0.2723042870514748f, 0.25794432128109435f, 0.5701857603000029f},
    {-0.3738971203301937f, 0.029763081771971678f, 0.5701857603000028f},
    {-0.6002455497652351f, 0.04605054446320633f, 0.4658359213500126f},
    {-0.638543739177529f, 0.284297989506008f, 0.401344174158341f},
    {-0.43586489250686666f, 0.41525554558404737f, 0.46583592135001267f},
    {-0.3331064404900414f, 0.5062111766415439f, 0.2571362434500323f},
    {-0.10603761905692632f, 0.4051136238003195f, 0.23250258801667933f},
    {-0.10487200031881994f, 0.26788801534869344f, 0.023802910116698883f},
    {-0.06657381090652603f, 0.02964057030589179f, 0.0882946573083705f},
    {-0.2692526575771884f, -0.1013169857721476f, 0.023802910116698772f},
    {-0.3720111095940137f, -0.19227261682964414f, 0.23250258801667922f},
    {-0.5990799310271288f, -0.09117506398841974f, 0.2571362434500322f},
    {-0.636657728441349f, 0.06226229090439217f, 0.06366100187501744f},
    {-0.6610477032038016f, 0.294317399823656f, 0.152786404500042f},
    {-0.4722770711829806f, 0.43146729202523315f, 0.06366100187501744f},
    {-0.3312204297538614f, 0.2841754780399281f, -0.08054692883329129f},
    {-0.4328132630325803f, 0.055994238530805415f, -0.0805469288332914f},
    // group 1, dodecahedron 1
    {-0.516346918063234f, 0.45108845201347686f, 0.0005134288416870358f},
    {-0.705117550084055f, 0.3139385598118998f, 0.08963883146671159f},
    {-0.6807275753216026f, 0.08188345089263593f, 0.0005134288416869248f},
    {-0.47688310991283367f, 0.07561539851904908f, -0.1436945018666218f},
    {-0.37529027663411485f, 0.3037966380281718f, -0.1436945018666218f},
    {-0.38460641800861955f, 0.3924338023722718f, -0.3770278351999552f},
    {-0.49195694330078155f, 0.21903334309421307f, -0.521235765908264f},
    {-0.5489870752669881f, 0.023228801251430786f, -0.3770278351999552f},
    {-0.7973942419882692f, -0.002879844041322799f, -0.3770278351999552f},
    {-0.8788143487169284f, 0.0333707230351587f, -0.1436945018666218f},
    {-1.0256286821594907f, 0.23544331725152773f, -0.1436945018666218f},
    {-0.9182781568673286f, 0.4088437765295865f, 0.0005134288416870358f},
    {-0.8612480249011221f, 0.6046483183723688f, -0.1436945018666218f},
    {-0.6128408581798409f, 0.6307569636651223f, -0.1436945018666218f},
    {-0.5314207514511817f, 0.5945063965886408f, -0.3770278351999552f},
    {-0.7295075248465076f, 0.5459936687311636f, -0.5212357659082638f},
    {-0.7051175500840552f, 0.3139385598118998f, -0.6103611685332886f},
    {-0.8938881821048762f, 0.1767886676103227f, -0.521235765908264f},
    {-1.0349448235339953f, 0.32408048159562775f, -0.3770278351999552f},
    {-0.9333519902552765f, 0.5522617211047505f, -0.3770278351999552f},
    // group 1, dodecahedron 2
    {-0.9379580289851295f, 0.1964098275985664f, -0.5843833389415947f},
    {-0.7491873969643085f, 0.33355971980014354f, -0.6735087415666192f},
    {-0.7735773717267611f, 0.5656148287194074f, -0.5843833389415947f},
    {-0.9774218371355299f, 0.5718828810929941f, -0.4401754082332858f},
    {-1.0790146704142487f, 0.34370164158387145f, -0.4401754082332858f},
    {-1.3053630998492902f, 0.3599891042751061f, -0.544525247183276f},
    {-1.343661289261584f, 0.5982365493179078f, -0.6090169943749477f},
    {-1.1409824425909216f, 0.7291941053959472f, -0.544525247183276f},
    {-1.0382239905740964f, 0.8201497364534437f, -0.7532249250832563f},
    {-0.8111551691409813f, 0.7190521836122192f, -0.7778585805166094f},
    {-0.8099895504028749f, 0.5818265751605932f, -0.9865582584165897f},
    {-0.7716913609905811f, 0.34357913011779156f, -0.922066511224918f},
    {-0.9743702076612435f, 0.21262157403975218f, -0.9865582584165897f},
    {-1.0771286596780687f, 0.12166594298225564f, -0.7778585805166094f},
    {-1.3041974811111838f, 0.22276349582348004f, -0.7532249250832563f},
    {-1.341775278525404f, 0.37620085071629195f, -0.9467001666582711f},
    {-1.3661652532878565f, 0.6082559596355558f, -0.8575747640332465f},
    {-1.1773946212670356f, 0.7454058518371329f, -0.9467001666582711f},
    {-1.0363379798379164f, 0.5981140378518279f, -1.09090809736658f},
    {-1.1379308131166352f, 0.3699327983427052f, -1.09090809736658f},
    // group 2, dodecahedron 0
    {0.21820310722186245f, -0.14288140365099833f, 0.42597782959169406f},
    {0.005042500438588823f, -0.047976186933311626f, 0.3368524269666695f},
    {-0.1837281315822322f, -0.1851260791348888f, 0.425977829591694f},
    {-0.08723419146562528f, -0.36479459078653426f, 0.5701857603000029f},
    {0.16117297525565585f, -0.33868594549378056f, 0.5701857603000028f},
    {0.26024183351937613f, -0.5428531668368533f, 0.46583592135001267f},
    {0.07306258843171992f, -0.6951440943082488f, 0.401344174158341f},
    {-0.1416894052847186f, -0.5850978423207437f, 0.4658359213500126f},
    {-0.27183851840616813f, -0.541584227949357f, 0.2571362434500322f},
    {-0.29781988010178573f, -0.2943880837602747f, 0.23250258801667922f},
    {-0.17956182650195415f, -0.22476582409613433f, 0.023802910116698772f},
    {0.007617418585702092f, -0.07247489662473888f, 0.0882946573083705f},
    {0.2223694123021406f, -0.18252114861224392f, 0.023802910116698883f},
    {0.3525185254235902f, -0.22603476298363062f, 0.23250258801667922f},
    {0.3784998871192077f, -0.4732309071727129f, 0.2571362434500323f},
    {0.2644081385996542f, -0.5824929117980988f, 0.06366100187501744f},
    {0.07563750657883318f, -0.719642803999676f, 0.152786404500042f},
    {-0.13752310020444045f, -0.6247375872819892f, 0.06366100187501744f},
    {-0.08049296823823385f, -0.428933045439207f, -0.08054692883329129f},
    {0.16791419848304728f, -0.4028244001464533f, -0.0805469288332914f},
    // group 2, dodecahedron 1
    {-0.13248059976585164f, -0.6727137742153009f, 0.0005134288416870358f},
    {0.080680007017422f, -0.7676189909329877f, 0.08963883146671159f},
    {0.26945063903824307f, -0.6304690987314105f, 0.0005134288416870358f},
    {0.17295669892163618f, -0.45080058707976506f, -0.1436945018666218f},
    {-0.07545046779964494f, -0.47690923237251875f, -0.1436945018666218f},
    {-0.1475544331537995f, -0.529295829640137f, -0.3770278351999552f},
    {0.05629003225496948f, -0.535563882013724f, -0.521235765908264f},
    {0.2543768056502953f, -0.48705115415624667f, -0.3770278351999552f},
    {0.4011911390928575f, -0.6891237483726157f, -0.3770278351999552f},
    {0.4105072804673624f, -0.7777609127167157f, -0.1436945018666218f},
    {0.30891444718864347f, -1.0059421522258383f, -0.1436945018666218f},
    {0.1050699817798745f, -0.9996740998522514f, 0.0005134288416870358f},
    {-0.0930167916154513f, -1.0481868277097286f, -0.1436945018666218f},
    {-0.23983112505801346f, -0.8461142334933597f, -0.1436945018666218f},
    {-0.2491472664325184f, -0.7574770691492597f, -0.3770278351999552f},
    {-0.10809062500339907f, -0.9047688831345648f, -0.521235765908264f},
    {0.08068000701742198f, -0.7676189909329877f, -0.6103611685332886f},
    {0.2938406138006956f, -0.8625242076506745f, -0.521235765908264f},
    {0.23681048183448894f, -1.0583287494934566f, -0.3770278351999552f},
    {-0.011596684886792177f, -1.0844373947862103f, -0.3770278351999552f},
    // group 2, dodecahedron 2
    {0.29888311423928443f, -0.910500394583986f, -0.5843833389415947f},
    {0.08572250745601082f, -0.8155951778662993f, -0.6735087415666192f},
    {-0.1030481245648102f, -0.9527450700678765f, -0.5843833389415947f},
    {-0.006554184448203285f, -1.132413581719522f, -0.4401754082332858f},
    {0.24185298227307783f, -1.1063049364267683f, -0.4401754082332858f},
    {0.34092184053679814f, -1.310472157769841f, -0.544525247183276f},
    {0.1537425954491419f, -1.4627630852412363f, -0.6090169943749477f},
    {-0.06100939826729661f, -1.3527168332537314f, -0.544525247183276f},
    {-0.19115851138874618f, -1.3092032188823446f, -0.7532249250832563f},
    {-0.21713987308436372f, -1.0620070746932624f, -0.7778585805166094f},
    {-0.09888181948453216f, -0.992384815029122f, -0.9865582584165897f},
    {0.08829742560312409f, -0.8400938875577266f, -0.922066511224918f},
    {0.3030494193195626f, -0.9501401395452316f, -0.9865582584165897f},
    {0.43319853244101214f, -0.9936537539166184f, -0.7778585805166094f},
    {0.4591798941366297f, -1.2408498981057006f, -0.7532249250832563f},
    {0.3450881456170762f, -1.3501119027310864f, -0.9467001666582711f},
    {0.15631751359625518f, -1.4872617949326636f, -0.8575747640332465f},
    {-0.056843093187018454f, -1.392356578214977f, -0.9467001666582711f},
    {0.00018703877918814504f, -1.1965520363721946f, -1.09090809736658f},
    {0.2485942055004693f, -1.170443391079441f, -1.09090809736658f},
};

uint8_t const ddh_dodecahedron_vertex_adjacency[
        DDH_VERTICES_PER_DODECAHEDRON][3] = {
    { 1,  4, 13},
    { 0,  2, 11},
    { 1,  3,  9},
    { 2,  4,  7},
    { 0,  3,  5},
    { 4,  6, 14},
    { 5,  7, 16},
    { 3,  6,  8},
    { 7,  9, 17},
    { 2,  8, 10},
    { 9, 11, 18},
    { 1, 10, 12},
    {11, 13, 19},
    { 0, 12, 14},
    { 5, 13, 15},
    {14, 16, 19},
    { 6, 15, 17},
    { 8, 16, 18},
    {10, 17, 19},
    {12, 15, 18},
};

uint8_t const ddh_dodecahedron_vertex_opposition[
        DDH_VERTICES_PER_DODECAHEDRON] = {
    17, 16, 15, 19, 18, 10, 11, 12, 13, 14, 5, 6, 7, 8, 9, 2, 1, 0, 4, 3,
};

uint8_t const ddh_dodecahedron_face_vertices[DDH_FACES_PER_DODECAHEDRON][
        DDH_VERTICES_PER_FACE] = {
    { 0,  4,  3,  2,  1},
    { 3,  4,  5,  6,  7},
    { 2,  3,  7,  8,  9},
    { 1,  2,  9, 10, 11},
    { 0,  1, 11, 12, 13},
    { 0, 13, 14,  5,  4},
    { 5, 14, 15, 16,  6},
    {12, 19, 15, 14, 13},
    {10, 18, 19, 12, 11},
    { 8, 17, 18, 10,  9},
    { 6, 16, 17,  8,  7},
    {15, 19, 18, 17, 16},
};

uint8_t const ddh_dodecahedron_vertex_faces[DDH_VERTICES_PER_DODECAHEDRON][
        DDH_VERTICES_PER_FACE] = {
    { 0,  4,  5}, { 0,  3,  4}, { 0,  2,  3}, { 0,  1,  2},
    { 0,  1,  5}, { 1,  5,  6}, { 1,  6, 10}, { 1,  2, 10},
    { 2,  9, 10}, { 2,  3,  9}, { 3,  8,  9}, { 3,  4,  8},
    { 4,  7,  8}, { 4,  5,  7}, { 5,  6,  7}, { 6,  7, 11},
    { 6, 10, 11}, { 9, 10, 11}, { 8,  9, 11}, { 7,  8, 11},
};

uint8_t const ddh_dodecahedron_face_adjacency[DDH_FACES_PER_DODECAHEDRON][5] = {
    { 1,  2,  3,  4,  5},
    { 0,  2,  5,  6, 10},
    { 0,  1,  3,  9, 10},
    { 0,  2,  4,  8,  9},
    { 0,  3,  5,  7,  8},
    { 0,  1,  4,  6,  7},
    { 1,  5,  7, 10, 11},
    { 4,  5,  6,  8, 11},
    { 3,  4,  7,  9, 11},
    { 2,  3,  8, 10, 11},
    { 1,  2,  6,  9, 11},
    { 6,  7,  8,  9, 10},
};

uint8_t const ddh_dodecahedron_face_opposition[DDH_FACES_PER_DODECAHEDRON] = {
    11, 8, 7, 6, 10, 9, 3, 2, 1, 5, 4, 0,
};

size_t ddh_group_dodecahedron_vertex_offsets[DDH_TOTAL_GROUPS + 1][
        DDH_DODECAHEDRONS_PER_GROUP][DDH_VERTICES_PER_DODECAHEDRON];

//uint8_t const ddh_group_adjacencies[DDH_TOTAL_GROUPS][2];
uint8_t const ddh_dodecahedron_adjacencies[DDH_TOTAL_DODECAHEDRONS] = {
};

uint8_t const ddh_light_dodecahedron[DDH_TOTAL_VERTICES] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
};

uint8_t const ddh_light_vertex[DDH_TOTAL_VERTICES] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
};

uint8_t const ddh_light_group[DDH_TOTAL_VERTICES] = {
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
     2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
};

