-- 002_seed_data.sql
-- Test/seed data for the ODB-generated Gymgym schema.
-- Run this after the schema SQL files / 001_schema.sql.
-- The insert order respects all foreign key dependencies.

BEGIN;

-- ---------------------------------------------------------------------------
-- Users
-- ---------------------------------------------------------------------------
INSERT INTO "users" ("id", "name", "email", "password_hash", "created_at") VALUES
  (1, 'Hermann Member', 'hermann.member@example.com', '$2b$12$examplehashhermann', '2026-05-01 09:00:00'),
  (2, 'Lisa Trainer', 'lisa.trainer@example.com', '$2b$12$examplehashlisa', '2026-05-02 10:00:00'),
  (3, 'Max Member', 'max.member@example.com', '$2b$12$examplehashmax', '2026-05-03 11:00:00');

-- ---------------------------------------------------------------------------
-- Muscles
-- ---------------------------------------------------------------------------
INSERT INTO "muscles" ("id", "name", "description") VALUES
  (1, 'Chest', 'Pectoralis major and minor.'),
  (2, 'Back', 'Latissimus dorsi, traps and upper back.'),
  (3, 'Quadriceps', 'Front thigh muscles used in squats and leg presses.'),
  (4, 'Hamstrings', 'Posterior thigh muscles used in hip hinges and deadlifts.'),
  (5, 'Shoulders', 'Deltoid muscles used in pressing and raising movements.'),
  (6, 'Biceps', 'Upper arm pulling muscle.'),
  (7, 'Triceps', 'Upper arm pushing muscle.'),
  (8, 'Core', 'Abdominal and stabilizing trunk muscles.'),
  (9, 'Glutes', 'Hip extension muscles.'),
  (10, 'Calves', 'Lower leg muscles.');

-- ---------------------------------------------------------------------------
-- Equipment
-- ---------------------------------------------------------------------------
INSERT INTO "equipment" ("id", "name", "description") VALUES
  (1, 'Barbell', 'Olympic barbell with plates.'),
  (2, 'Dumbbells', 'Pair of adjustable or fixed dumbbells.'),
  (3, 'Cable Machine', 'Cable stack for rows, pulldowns and isolation work.'),
  (4, 'Bodyweight', 'No external equipment required.'),
  (5, 'Leg Press Machine', 'Machine for lower-body pressing movements.'),
  (6, 'Bench', 'Flat or adjustable bench.'),
  (7, 'Pull-up Bar', 'Fixed bar for pull-ups and hanging exercises.');

-- ---------------------------------------------------------------------------
-- Exercises
-- intensity_level / difficulty_level convention used here:
-- 1 = low/beginner, 2 = moderate/intermediate, 3 = high/advanced.
-- ---------------------------------------------------------------------------
INSERT INTO "exercises"
  ("id", "name", "description", "intensity_level", "difficulty_level",
   "primary_muscle", "secondary_muscle", "equipment")
VALUES
  (1, 'Bench Press', 'Classic barbell chest press performed on a flat bench.', 3, 2, 1, 7, 1),
  (2, 'Squat', 'Barbell back squat for legs and glutes.', 3, 2, 3, 9, 1),
  (3, 'Deadlift', 'Barbell hip-hinge movement for posterior chain strength.', 3, 3, 4, 2, 1),
  (4, 'Pull-up', 'Bodyweight vertical pulling movement.', 3, 2, 2, 6, 7),
  (5, 'Shoulder Press', 'Standing or seated overhead press.', 2, 2, 5, 7, 2),
  (6, 'Cable Row', 'Horizontal pulling movement on a cable machine.', 2, 1, 2, 6, 3),
  (7, 'Leg Press', 'Machine-based leg press movement.', 2, 1, 3, 9, 5),
  (8, 'Plank', 'Static core stabilization exercise.', 1, 1, 8, NULL, 4),
  (9, 'Dumbbell Curl', 'Dumbbell biceps isolation exercise.', 1, 1, 6, NULL, 2),
  (10, 'Triceps Pushdown', 'Cable triceps isolation exercise.', 1, 1, 7, NULL, 3);

-- ---------------------------------------------------------------------------
-- Training plans
-- ---------------------------------------------------------------------------
INSERT INTO "training_plans"
  ("id", "name", "description", "duration_minutes", "difficulty_level",
   "created_at", "is_public", "user")
VALUES
  (1, 'Beginner Full Body', 'Simple full-body plan for new members.', 60, 1, '2026-05-04 08:00:00', TRUE, 2),
  (2, 'Strength Foundation', 'Compound-focused strength plan.', 75, 2, '2026-05-05 08:00:00', TRUE, 2),
  (3, 'Niklas Custom Push Day', 'Custom upper-body push workout.', 55, 2, '2026-05-06 08:00:00', FALSE, 1);

-- ---------------------------------------------------------------------------
-- Training plan exercises
-- ---------------------------------------------------------------------------
INSERT INTO "training_plan_exercises"
  ("id", "order", "notes", "training_plan", "exercise")
VALUES
  -- Beginner Full Body
  (1, 1, 'Focus on controlled depth and stable tempo.', 1, 2),
  (2, 2, 'Use assisted variation if needed.', 1, 4),
  (3, 3, 'Keep shoulder blades retracted.', 1, 1),
  (4, 4, 'Hold a strong neutral position.', 1, 8),

  -- Strength Foundation
  (5, 1, 'Main lower-body strength movement.', 2, 2),
  (6, 2, 'Main upper-body strength movement.', 2, 1),
  (7, 3, 'Keep spine neutral and brace hard.', 2, 3),
  (8, 4, 'Controlled pulling volume.', 2, 6),

  -- Hermann Custom Push Day
  (9, 1, 'Main push lift.', 3, 1),
  (10, 2, 'Secondary shoulder press.', 3, 5),
  (11, 3, 'Finish with triceps volume.', 3, 10);

-- ---------------------------------------------------------------------------
-- Planned sets
-- ---------------------------------------------------------------------------
INSERT INTO "planned_sets"
  ("id", "trainingPlanExercise", "set_number", "target_repetitions",
   "target_weight_kg", "target_rest_seconds", "notes")
VALUES
  -- Beginner Full Body: Squat
  (1, 1, 1, 10, 40.0, 90, 'Warm-up working set.'),
  (2, 1, 2, 10, 40.0, 90, ''),
  (3, 1, 3, 10, 40.0, 90, ''),

  -- Beginner Full Body: Pull-up
  (4, 2, 1, 8, 0.0, 90, 'Bodyweight or assisted.'),
  (5, 2, 2, 8, 0.0, 90, ''),
  (6, 2, 3, 8, 0.0, 90, ''),

  -- Beginner Full Body: Bench Press
  (7, 3, 1, 10, 35.0, 90, ''),
  (8, 3, 2, 10, 35.0, 90, ''),
  (9, 3, 3, 10, 35.0, 90, ''),

  -- Beginner Full Body: Plank
  (10, 4, 1, 1, 45.0, 60, 'Target is 45 seconds.'),
  (11, 4, 2, 1, 45.0, 60, 'Target is 45 seconds.'),

  -- Strength Foundation
  (12, 5, 1, 5, 80.0, 180, ''),
  (13, 5, 2, 5, 80.0, 180, ''),
  (14, 5, 3, 5, 80.0, 180, ''),
  (15, 6, 1, 5, 60.0, 180, ''),
  (16, 6, 2, 5, 60.0, 180, ''),
  (17, 6, 3, 5, 60.0, 180, ''),
  (18, 7, 1, 5, 100.0, 180, ''),
  (19, 7, 2, 5, 100.0, 180, ''),
  (20, 7, 3, 5, 100.0, 180, ''),
  (21, 8, 1, 10, 45.0, 120, ''),
  (22, 8, 2, 10, 45.0, 120, ''),
  (23, 8, 3, 10, 45.0, 120, ''),

  -- Hermann Custom Push Day
  (24, 9, 1, 8, 65.0, 150, ''),
  (25, 9, 2, 8, 65.0, 150, ''),
  (26, 9, 3, 8, 65.0, 150, ''),
  (27, 10, 1, 10, 24.0, 120, ''),
  (28, 10, 2, 10, 24.0, 120, ''),
  (29, 10, 3, 10, 24.0, 120, ''),
  (30, 11, 1, 12, 25.0, 75, ''),
  (31, 11, 2, 12, 25.0, 75, ''),
  (32, 11, 3, 12, 25.0, 75, '');

-- ---------------------------------------------------------------------------
-- Workout sessions
-- status convention used here:
-- 0 = planned, 1 = in_progress, 2 = completed, 3 = cancelled.
-- ---------------------------------------------------------------------------
INSERT INTO "workout_sessions"
  ("id", "description", "started_at", "ended_at", "status", "user", "training_plan")
VALUES
  (1, 'Completed beginner full-body session.', '2026-05-10 18:00:00', '2026-05-10 19:05:00', 2, 1, 1),
  (2, 'Completed strength foundation session.', '2026-05-14 17:30:00', '2026-05-14 18:55:00', 2, 1, 2),
  (3, 'Planned push day for tomorrow.', NULL, NULL, 0, 1, 3),
  (4, 'Max free workout without a training plan.', '2026-05-15 09:30:00', '2026-05-15 10:10:00', 2, 3, NULL);

-- ---------------------------------------------------------------------------
-- Workout session exercises
-- ---------------------------------------------------------------------------
INSERT INTO "workout_session_exercises"
  ("id", "order", "notes", "workout_session", "exercise")
VALUES
  -- Session 1
  (1, 1, 'Felt stable.', 1, 2),
  (2, 2, 'Used assisted reps.', 1, 4),
  (3, 3, 'Good control.', 1, 1),
  (4, 4, 'Core finisher.', 1, 8),

  -- Session 2
  (5, 1, 'Heavy but controlled.', 2, 2),
  (6, 2, 'Bench moved well.', 2, 1),
  (7, 3, 'Grip was limiting.', 2, 3),
  (8, 4, 'Finished with rows.', 2, 6),

  -- Session 3 planned
  (9, 1, 'Upcoming push day.', 3, 1),
  (10, 2, 'Upcoming push day.', 3, 5),
  (11, 3, 'Upcoming push day.', 3, 10),

  -- Session 4 free workout
  (12, 1, 'Quick leg press work.', 4, 7),
  (13, 2, 'Arm finisher.', 4, 9);

-- ---------------------------------------------------------------------------
-- Performed sets
-- ---------------------------------------------------------------------------
INSERT INTO "performed_sets"
  ("id", "workoutSessionExercise", "plannedSet", "set_number",
   "repetitions", "weight_kg", "rest_seconds", "completed", "notes")
VALUES
  -- Session 1: Squat based on plan exercise 1
  (1, 1, 1, 1, 10, 40.0, 90, TRUE, ''),
  (2, 1, 2, 2, 10, 40.0, 90, TRUE, ''),
  (3, 1, 3, 3, 9, 40.0, 100, FALSE, 'Missed one rep.'),

  -- Session 1: Pull-up
  (4, 2, 4, 1, 8, 0.0, 90, TRUE, 'Assisted.'),
  (5, 2, 5, 2, 7, 0.0, 90, FALSE, 'Lost form.'),
  (6, 2, 6, 3, 6, 0.0, 100, FALSE, ''),

  -- Session 1: Bench Press
  (7, 3, 7, 1, 10, 35.0, 90, TRUE, ''),
  (8, 3, 8, 2, 10, 35.0, 90, TRUE, ''),
  (9, 3, 9, 3, 10, 35.0, 90, TRUE, ''),

  -- Session 1: Plank
  (10, 4, 10, 1, 1, 50.0, 60, TRUE, 'Held 50 seconds.'),
  (11, 4, 11, 2, 1, 45.0, 60, TRUE, 'Held 45 seconds.'),

  -- Session 2: Strength Foundation
  (12, 5, 12, 1, 5, 80.0, 180, TRUE, ''),
  (13, 5, 13, 2, 5, 80.0, 180, TRUE, ''),
  (14, 5, 14, 3, 5, 80.0, 180, TRUE, ''),
  (15, 6, 15, 1, 5, 60.0, 180, TRUE, ''),
  (16, 6, 16, 2, 5, 60.0, 180, TRUE, ''),
  (17, 6, 17, 3, 4, 60.0, 180, FALSE, 'Failed last rep.'),
  (18, 7, 18, 1, 5, 100.0, 180, TRUE, ''),
  (19, 7, 19, 2, 5, 100.0, 180, TRUE, ''),
  (20, 7, 20, 3, 5, 100.0, 180, TRUE, ''),
  (21, 8, 21, 1, 10, 45.0, 120, TRUE, ''),
  (22, 8, 22, 2, 10, 45.0, 120, TRUE, ''),
  (23, 8, 23, 3, 9, 45.0, 120, FALSE, 'Grip fatigue.'),

  -- Session 4 free workout, no planned set reference
  (24, 12, NULL, 1, 12, 120.0, 90, TRUE, ''),
  (25, 12, NULL, 2, 12, 120.0, 90, TRUE, ''),
  (26, 12, NULL, 3, 10, 130.0, 100, TRUE, ''),
  (27, 13, NULL, 1, 12, 12.5, 60, TRUE, ''),
  (28, 13, NULL, 2, 12, 12.5, 60, TRUE, ''),
  (29, 13, NULL, 3, 10, 12.5, 60, TRUE, '');

-- ---------------------------------------------------------------------------
-- Body measurements
-- ---------------------------------------------------------------------------
INSERT INTO "body_measurements"
  ("id", "measured_at", "weight_kg", "body_fat_percentage",
   "chest_cm", "waist_cm", "arm_cm", "leg_cm", "user")
VALUES
  (1, '2026-05-01 08:00:00', 78.5, 16.5, 102, 84, 36, 58, 1),
  (2, '2026-05-15 08:00:00', 78.1, 16.1, 103, 83, 36, 59, 1),
  (3, '2026-05-01 08:00:00', 91.0, 21.0, 110, 96, 39, 64, 3),
  (4, '2026-05-15 08:00:00', 90.4, 20.5, 110, 95, 39, 64, 3);

-- ---------------------------------------------------------------------------
-- Personal records
-- ---------------------------------------------------------------------------
INSERT INTO "personal_records"
  ("id", "weight_kg", "repetitions", "achieved_at", "user", "exercise")
VALUES
  (1, 75.0, 1, '2026-05-14 18:20:00', 1, 1),
  (2, 110.0, 1, '2026-05-14 18:45:00', 1, 3),
  (3, 90.0, 1, '2026-05-14 17:55:00', 1, 2),
  (4, 140.0, 10, '2026-05-15 09:50:00', 3, 7),
  (5, 15.0, 12, '2026-05-15 10:05:00', 3, 9);

-- ---------------------------------------------------------------------------
-- Reset BIGSERIAL sequences so future inserts continue after seeded IDs.
-- ---------------------------------------------------------------------------
SELECT setval(pg_get_serial_sequence('users', 'id'), COALESCE((SELECT MAX("id") FROM "users"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('muscles', 'id'), COALESCE((SELECT MAX("id") FROM "muscles"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('equipment', 'id'), COALESCE((SELECT MAX("id") FROM "equipment"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('exercises', 'id'), COALESCE((SELECT MAX("id") FROM "exercises"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('training_plans', 'id'), COALESCE((SELECT MAX("id") FROM "training_plans"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('training_plan_exercises', 'id'), COALESCE((SELECT MAX("id") FROM "training_plan_exercises"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('planned_sets', 'id'), COALESCE((SELECT MAX("id") FROM "planned_sets"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('workout_sessions', 'id'), COALESCE((SELECT MAX("id") FROM "workout_sessions"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('workout_session_exercises', 'id'), COALESCE((SELECT MAX("id") FROM "workout_session_exercises"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('performed_sets', 'id'), COALESCE((SELECT MAX("id") FROM "performed_sets"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('body_measurements', 'id'), COALESCE((SELECT MAX("id") FROM "body_measurements"), 1), TRUE);
SELECT setval(pg_get_serial_sequence('personal_records', 'id'), COALESCE((SELECT MAX("id") FROM "personal_records"), 1), TRUE);

COMMIT;
