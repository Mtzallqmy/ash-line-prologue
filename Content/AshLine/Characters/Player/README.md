# Prompt 02 — Player Blueprint Assets

## `BP_ALPlayerCharacter`

أنشئ Blueprint Class يرث من `AALPlayerCharacter`، وسمّه `BP_ALPlayerCharacter`. تقتصر مسؤولياته على تعيين Skeletal Mesh، وAnimation Blueprint، وCamera offsets، و`MovementSettings`، ومراجع المحتوى القابلة للضبط من المصمم. لا تنقل منطق الحركة أو الإدخال الأساسي إلى Blueprint.

## `DA_PlayerMovement`

أنشئ Primary Data Asset من نوع `UALPlayerMovementSettings` باسم `DA_PlayerMovement`، واضبط القيم الأولية التالية:

| الخاصية | القيمة الابتدائية |
|---|---:|
| WalkSpeed | 320 |
| SprintSpeed | 520 |
| CrouchSpeed | 180 |
| JumpVelocity | 420 |
| MinPitch | -85 |
| MaxPitch | 85 |
| InteractionDistance | 250 |
| HorizontalSensitivity | 1.0 |
| VerticalSensitivity | 1.0 |
| InvertY | false |
| SprintMode | Hold |

## `BP_ALPrologueGameMode`

أنشئ Blueprint GameMode مشتقًا من `AALPrologueGameMode`، واجعل `Default Pawn Class` هو `BP_ALPlayerCharacter`، و`Player Controller Class` هو `AALPlayerController` أو Blueprint مشتق منه. اربط هذا الـ GameMode بخريطة `L_Namar_Prologue` من إعدادات الخريطة أو `DefaultEngine.ini`.

## Animation Foundation

يستخرج Animation Blueprint من الشخصية `Speed` و`Direction` و`IsFalling` و`IsCrouching` و`IsSprinting` باستخدام الدوال العامة في `AALPlayerCharacter`. لا تنشئ مكتبة Footstep أو Animation System كاملًا في هذه المرحلة؛ اترك Hook المرحلة الصوتية معتمدًا على Movement State وPhysical Surface.
